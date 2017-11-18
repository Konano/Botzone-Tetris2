/*  

RESTE A FAIRE:
   - debugger :-(
   - un garbage collector qui loope sur les noeuds et destroye tout ce qui a moins de X simus
   - bandit: gérer les autres formules

   - possible optimisation ?: stocker dans les noeuds les pointeurs vers les fils (d'abord, reflechir
     a si c'est une bonne idee :-) )

*/

#include <time.h>
#include <math.h>
#include <gsl/gsl_sort_double.h>
#include "config.h"
#include "uct.h"
#include "game.h"
#include "hashtable.h"
#include "feature_policy.h"
#include "random.h"
#include "macros.h"

#define NB_SIMU_BEFORE_CREATION 1
#define HASHTABLE_SIZE 512
#define DEPTH_MAX 100

#define MAX_ACTIONS 34 /* depends on the board width and the shape of the pieces */
#define NB_ALEA 7 /* number of possible random outcomes */

typedef struct UctNodeStats
{
  double sum_rewards;
  int nb_simu;  
} UctNodeStats;

struct UctNode
{
  /* current node */
  UctNodeStats stats;
  
  int nb_possible_actions;
  Action possible_actions[MAX_ACTIONS];
  
  /* sons */
  UctNodeStats sons_stats[MAX_ACTIONS][NB_ALEA];
};

/* FIXME: pas en variable globale */
static Hashtable *hashtable;

static const char* compute_key(Game *game);
static double bandit(int nb_trials, int total_trials, double sum_rewards);
static double simu_and_update(UctNode *root, Game *game,
			      double (*valueEstimator)(Game *game), double (*heuristic)(Game *game));
static int get_ucb_action(UctNode *father, Game *game);
static void cumulate_ucb_stats(UctNode *father, Game *son, int num_action,
			       int num_random, UctNodeStats *uct_node_stats);

static double heuristic(Game *game);
/* static double value_estimator(Game *game); */
/* static double monte_carlo_value_estimator(Game *game);*/



/**
 * @brief Computes a string key representing a game state.
 *
 * You never have to free the string returned.
 *
 * @param game the game state
 */
static const char* compute_key(Game *game)
{
  static char key[256];
  Board *board;
  size_t size;

  board = game->board;
  size = board->height * 2;

  MEMCPY(key, board->rows, char, size);
  key[size] = game->current_piece_index + 1;
  key[size + 1] = '\0';

  /* debug
  if (strlen(key) != size + 1) {
    DIE2("Computed a bad key: %s (length = %d)\n", key, strlen(key));
  }*/

  return key;
}

/**
 * @brief Returns a score in order to decide what action to choose
 * (exploration vs exploitation dilemma)
 * @param nb_trials number of times the move has already been tried
 * @param total_trials total number of trials
 * @param sum_rewards sum of the rewards obtained with this move
 */
static double bandit(int nb_trials, int total_trials, double sum_rewards)
{
  if (nb_trials == 0)
    return TETRIS_INFINITE;

  return sum_rewards / (double) nb_trials + /* exploitation term*/
    sqrt(log((double) total_trials) / (double) nb_trials); /* exploration term */
}


/**
 * @brief Develops the tree from the game configuration and returns the most simulated action.
 * @param root root node of the tree
 * @param game game configuration (wall+piece) from which to develop the tree
 * @param time time in miliseconds to think
 * @param value_estimator a function estimating the expected reward of a state
 * @param heuristic a function indicating the quality of a state, used to sort the actions
 * @return the most simulated action
 */
Action think(UctNode *root, const Game *game, double time,
	     double (*value_estimator)(Game *game), double (*heuristic)(Game *game))
{
  int nb_simu_max = 0;
  int i, j;
  int best_action = -1;
  clock_t end_time = clock() + time * CLOCKS_PER_SEC / 1000;
  Action action;
  UctNodeStats stats;

  Game *gamecopy;


  /* develops the tree */
  i=0;
  while (clock() < end_time)    {
      gamecopy=new_game_copy(game);
      simu_and_update(root, gamecopy, value_estimator, heuristic);
      i++;
      free_game(gamecopy);
  }
  printf("Launched %d simulations\n",i);

  gamecopy=new_game_copy(game);

  /* find the most simulated action */
  for (i = 0; i < root->nb_possible_actions; i++) {
    stats.nb_simu = 0; 
    stats.sum_rewards = 0; 
    game_drop_piece(gamecopy,&(root->possible_actions[i]),1);
    
    for (j = 0; j < NB_ALEA; j++) {      
      game_set_current_piece_index(gamecopy,j);     
      
      cumulate_ucb_stats(root, gamecopy, i, j, &stats);
    }   

    game_cancel_last_move(gamecopy);
 
    if (stats.nb_simu > nb_simu_max) {
      best_action = i;
      nb_simu_max = stats.nb_simu;
    }
  }
  free_game(gamecopy);

  printf("Best action: action=%d (nbsimu=%d)\n", best_action, nb_simu_max);
  
  if (best_action==-1) { /* there is no possible action -> game is over and root->possible_actions is empty */
    action.column=1;
    action.orientation=0;
  }
  else {
    action = root->possible_actions[best_action];
  }

  return action;
}

/**
 * @brief Simulates and provides an estimated average reward.
 * @param root the root node
 * @param game the game
 * @param value_estimator a function estimating the expected reward of a state
 * @param heuristic a function indicating the quality of a state, used to sort the actions
 * @return a prediction of the future score from root/game
 */
static double simu_and_update(UctNode *root, Game *game,
			      double (*value_estimator)(Game *game), double(*heuristic)(Game *game)) {


  UctNode *current_node = root;
  double reward;
  int action_index;
  Action action;

  int index = 0;
  int i;
  const char *key = NULL;

  /* stacks for updating the stats at the end of the current simulation */
  static UctNode *node_stack[DEPTH_MAX];
  static int action_stack[DEPTH_MAX];
  static int score_stack[DEPTH_MAX];
  static int alea_stack[DEPTH_MAX];
  static UctNodeStats last_node_stats;

  /* simulates in the tree */
  while ((current_node != NULL) && (!game->game_over)) {

      if (index >= DEPTH_MAX)	{
	  DIE("index > DEPTH_MAX; increase DEPTH_MAX or debug :-)"); 
      }

      /* let's choose the action */
      action_index = get_ucb_action(current_node, game);

      /* for the forthcoming update, we store the visited nodes, actions  */
      node_stack[index] = current_node;
      action_stack[index] = action_index;
      score_stack[index] = game->score;      

      /* let's play the action */
      if (action_index != -1) {
	action = current_node->possible_actions[action_index];
      }
      else { /* game over case (no possible action: current_node->possible_actions is empty) */
	action.orientation = 0;
	action.column = 1;
      }

      game_drop_piece(game, &action, 0);
      alea_stack[index] = game->current_piece_index;

      index++;

      /* let's look for the future node */
      key = compute_key(game);
      current_node = (UctNode*) hashtable_get(hashtable, key);
  }



  /* we estimate the total reward */
  /* remark: in case of gameover, value_estimator should return 0 (the flag -evaluate_gameover_with_features should not be ON) */
  reward = game->score + value_estimator(game); 
        
  /* we update the statistics in the tree */
  for (i = 0; i < index; i++) {
       
    node_stack[i]->stats.nb_simu++;
    node_stack[i]->stats.sum_rewards += reward - score_stack[i];

    /*    printf("-> node=%p (score=%i, value=%f) a=%i alea=%i nbsimu=%i sumv=%f",(void*)node_stack[i],score_stack[i],reward - score_stack[i],action_stack[i],alea_stack[i],node_stack[i]->stats.nb_simu,node_stack[i]->stats.sum_rewards); */
    
  }

  
  /* special treatment for the last move */
  
  if (!game->game_over) {  /* <- we went out of the while loop because one found an unknown node -> one should consider creating it */

    i=index-1; /* i =  index of the last existing node in the trajectory */
    last_node_stats=node_stack[i]->sons_stats[action_stack[i]][alea_stack[i]];
      
    if (last_node_stats.nb_simu >= NB_SIMU_BEFORE_CREATION-1) { /* do we create a new node ? (remark: we have not counted yet the last move) */
      
      current_node = uct_node_new(game, heuristic);
      hashtable_add(hashtable, key, current_node); 

      /* printf("-> NewNode=%p ",(void*)current_node); */
     
      current_node->stats=last_node_stats;  /* the new son inherits the stats of its creating father (this is approximate since a node could have several fathers) */ 

      current_node->stats.nb_simu++;
      current_node->stats.sum_rewards += reward - game->score;

      /*      printf("nbsim=%d sumv=%f ",current_node->stats.nb_simu,current_node->stats.sum_rewards);*/

    } else {  /* node was not created -> count the statistics only in the last node */
      /*      printf("-> NoNewNode ");*/

      node_stack[i]->sons_stats[action_stack[i]][alea_stack[i]].nb_simu++;
      node_stack[i]->sons_stats[action_stack[i]][alea_stack[i]].sum_rewards += reward - game->score;

      /*      printf("nbsim=%d sumv=%f ",node_stack[i]->sons_stats[action_stack[i]][alea_stack[i]].nb_simu,node_stack[i]->sons_stats[action_stack[i]][alea_stack[i]].sum_rewards ); */

    }
  } 

  /*  printf("\n");*/

  return reward - score_stack[0];
}



static int get_ucb_action(UctNode *father, Game *game) {
  
  /* the local variable stats will contain the sum of the stats for all the 7 possible next pieces/son */

  UctNodeStats stats[MAX_ACTIONS];
  Action action;
  int i, j, best_action;
  int total_trials = 0;
  double best_ucb_value,ucb_value;
  double stop;

  /* cumulate the stats */

  for (i = 0; i < father->nb_possible_actions; i++) {

      stats[i].nb_simu = 0;     
      stats[i].sum_rewards = 0; 

      action = father->possible_actions[i];

      /* /\* debug *\/ */
/*       if (action.column <= 0 || action.column > game_get_nb_possible_columns(game, action.orientation) */
/* 	  || action.orientation < 0 || action.orientation >= game_get_nb_possible_orientations(game)) { */

/* 	printf("Incorrect action: column %d, orientation %d\n", action.column, action.orientation); */
/* 	game_print(stdout, game); */

/* 	printf("The %d possible actions are:\n", father->nb_possible_actions); */

/* 	for (i = 0; i < father->nb_possible_actions; i++) { */
/* 	  action = father->possible_actions[i]; */
/* 	  printf("%d %d\n", action.column, action.orientation); */
/* 	} */
/* 	exit(1); */
/*       } */

      game_drop_piece(game, &action, 1);

      if (game->game_over) { /* debug (should never happen) */
	  DIE("The father contains a gameover action!");
      }

      for (j = 0; j < NB_ALEA; j++) {  
	game_set_current_piece_index(game, j);
	cumulate_ucb_stats(father, game, i, j, &stats[i]);
      }
      total_trials += stats[i].nb_simu;
     
      game_cancel_last_move(game);
  }

  /* printf("total trials = %d\n", total_trials); */

  /* find the best action */

  best_action = -1;
  best_ucb_value = -TETRIS_INFINITE;
  stop = MIN(father->nb_possible_actions, MAX(1,sqrt((double) total_trials)));
/*   printf("total_trials=%i stop=%f\n",total_trials,stop); */

  for (i = 0; i < stop; i++) {
    ucb_value = bandit(stats[i].nb_simu, total_trials, stats[i].sum_rewards);
    /*       printf("%f ",ucb_value); */
    if (ucb_value > best_ucb_value) {
      best_ucb_value = ucb_value;
      best_action = i;
    }
  }
/*   printf("\n"); */

  /* debug */
  if ((best_action==-1) && (father->nb_possible_actions!=0)) {
    DIE3("AARRgh: best_action=-1 but there seems to be possible actions: %i (stop=%f, totaltrials=%i)\n", father->nb_possible_actions,stop,total_trials);
  }

  return best_action;
}

/**
 * For given father and son,
 * and the action numero_action which relates the father to the son,
 * and the random outcome numero_random,
 * we add the sum_rewards and the number of trials to the uct_node_stats.
 */
static void cumulate_ucb_stats(UctNode *father, Game *son, int num_action,
			       int num_random, UctNodeStats *uct_node_stats) {

  UctNode *node;

  node = hashtable_get(hashtable, compute_key(son));

  if (node != NULL) {
    uct_node_stats->nb_simu += node->stats.nb_simu;
    uct_node_stats->sum_rewards += node->stats.sum_rewards;
  }
  else {
    uct_node_stats->nb_simu += father->sons_stats[num_action][num_random].nb_simu;
    uct_node_stats->sum_rewards += father->sons_stats[num_action][num_random].sum_rewards;
  }

}

/****
 UctNodeStats *uct_node_get_son_stats(UctNode *node, int son_index) {
  if (node->sons[son_index]==NULL) {
    return &node->sons_stats[son_index];
  } else {
    return &node->sons[son_index].stats;
  }
  }*/

UctNode *uct_node_new(Game *game, double (*heuristic)(Game *game)) {

  UctNode *p;
  static double evaluations[MAX_ACTIONS];
  static size_t sorted_indexes[MAX_ACTIONS];
  static Action actions[MAX_ACTIONS];

  int i, j, k;
  int nb_possible_orientations, nb_possible_columns;
  Action action;

  CALLOC(p, UctNode, 1);

  k = 0;

  nb_possible_orientations = game_get_nb_possible_orientations(game);
  for (i = 0; i < nb_possible_orientations; i++) {
    action.orientation = i;
    nb_possible_columns = game_get_nb_possible_columns(game, i);

    for (j = 1; j <= nb_possible_columns; j++) {
      action.column = j;

      game_drop_piece(game, &action, 1);
      evaluations[k] = heuristic(game);

      if (!game->game_over) {
	actions[k] = action;
	k++;
      }

      game_cancel_last_move(game);
    }
  }

  p->nb_possible_actions = k; /* note that nb_possible_actions can be zero */

  /* sort the actions */
  gsl_sort_index(sorted_indexes, evaluations, 1, k);
  for (i = 0; i < k; i++) {
    p->possible_actions[i] = actions[sorted_indexes[k - 1 - i]];
    /*    printf("(%i) %i %i %f  ",i,p->possible_actions[i].column,p->possible_actions[i].orientation,evaluations[sorted_indexes[k-1-i]]); */
  }

  return p;
}

void uct_node_free(void *node) {

  FREE(node);
}




/* static double value_estimator(Game *game) { */

/*   static FeaturePolicy *feature_policy = NULL; */

/*   if (feature_policy == NULL) { */
/*     MALLOC(feature_policy, FeaturePolicy); */
/*     load_feature_policy("features/value_estimator_bertsekas.dat", feature_policy); */
/*     /\* load_feature_policy("features/record_du.dat", feature_policy); *\/ /\* hack pour utiliser la fonction heuristique -> a l'air de mieux marcher *\/ */
/*   } */
/*   return evaluate_features(game, feature_policy); */

/* } */


static double heuristic(Game *game) {

  static FeaturePolicy *feature_policy = NULL;

  if (feature_policy == NULL) {
    MALLOC(feature_policy, FeaturePolicy);
    load_feature_policy("features/record_du.dat", feature_policy);
  }
  return evaluate_features(game, feature_policy);

}



/* static double monte_carlo_value_estimator(Game *game) { */

/*   static FeaturePolicy *feature_policy = NULL; */
/*   Action action; */
/*   int initial_score, value; */
/*   Game *game2; */

/*   if (feature_policy == NULL) { */
/*     MALLOC(feature_policy, FeaturePolicy); */
/*     load_feature_policy("features/record_bdu.dat", feature_policy); */
/*   } */

/*   game2 = new_game_copy(game); */

   /* just finish the game */ 
/*   initial_score = game2->score; */
  
/*   while (!game2->game_over) { */

/*     features_get_best_action(game2, feature_policy, &action); */
/*     game_drop_piece(game2, &action, 0); */
/*   } */

/*   value = game2->score - initial_score; */
/*    printf("value = %i\n",value);*/ 
 
/*   free_game(game2); */

/*   return  value; */
/* } */





int main(int argc, char **argv) {

  Game *game;
  UctNode *root;
  Action action;

  initialize_random_generator(time(NULL));
  /*  initialize_random_generator(1); to make our bugs reproductible :) */

  hashtable = hashtable_new(HASHTABLE_SIZE, uct_node_free); 

  /* Play one game */

  game = new_game(0, 10, 10, 0, "pieces4.dat", NULL);

  root = uct_node_new(game, heuristic);
  hashtable_add(hashtable, compute_key(game), root);
  
  while (!game->game_over) {
    action = think(root, game, 10, heuristic, heuristic);
    game_drop_piece(game, &action, 0);
    game_print(stdout, game);
    /*    getchar(); */

    root = (UctNode*) hashtable_get(hashtable, compute_key(game));

    if (root == NULL && !game->game_over) {  /* it might be that the new game state we get to was not created by UCT: this happens if random simulations never picked the right "next piece" */
      root = uct_node_new(game, heuristic);
      hashtable_add(hashtable, compute_key(game), root);
    }
  }
  
  free_game(game);
  exit_random_generator();

  hashtable_free(hashtable);

  return 0;
}
