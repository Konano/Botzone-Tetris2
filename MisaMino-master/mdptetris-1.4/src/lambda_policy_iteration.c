/**
 * This module executes the lambda policy iteration algorithm.
 *
 * Usage: ./lambda_policy_iteration ...
 *
 * You can stop the algorithm with Ctrl-C. The current iteration
 * is then terminated. The policy is saved
 * into the feature based policy file you specified in the command line.
 *
 * This feature file can be used to continue the execution later
 * or to play in the simulator (./tetris).
 */

#include <stdio.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_multifit.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "config.h"
#include "lambda_policy_iteration_parameters.h"
#include "feature_functions.h"
#include "rewards.h"
#include "games_statistics.h"
#include "interruptions.h"
#include "random.h"
#include "macros.h"


/**
 * The following structures allow to store all the information about the batch of games
 * we are playing in one iteration of the algorithm.
 * Once the batch is played, this information are used to build the matrix and the vector
 * of the least-squares problem to be solved.
 */

/**
 * Information about one state visited during a game.
 */
typedef struct StateHistory {
  double state_value;     /* value function in this state */
  int reward;             /* immediate reward obtained in this state */
  double *feature_values; /* set of features for this state (matrix a) */
} StateHistory;

/**
 * All moves in a game.
 */
typedef struct GameHistory {
  StateHistory *states;  /* the states visited during the game */
  int nb_visited_states; /* number of states in the game */
  int capacity;          /* memory allocated for the states
			    (used to know when a realloc is needed) */
} GameHistory;

/* functions to start / end the algorithm */
static void initialize_lambda_policy_iteration(LPIParameters *parameters);
static void exit_lambda_policy_iteration(LPIParameters *parameters);

/* the algorithm itself */
static void lambda_policy_iteration(LPIParameters *parameters);

/* first step of the algorithm: play a batch of games */
static void play_game(Game *game, LPIParameters *parameters, GameHistory *game_history);
static void play_step(Game *game, const LPIParameters *parameters, StateHistory *state_history);

/* second step: update the weights */
static void linear_solve(gsl_matrix *ata, gsl_vector *atb, gsl_vector *x);
static double update_weights(LPIParameters *parameters, gsl_matrix *ata, gsl_vector *atb, int nb_iterations);
static void update_matrices(const LPIParameters *parameters, gsl_matrix *ata, gsl_vector *atb,
			    GameHistory *game_history);


/******** BEGINNING OF DEBUG FUNCTIONS ***************/


/**
 * Prints the game history (usefull for debugging)
 */

/* static void game_print_history(LPIParameters *parameters,GameHistory *game_history) { */
  
/*   int i,j; */

/*   printf("\n"); */
/*   for (i=0; i<game_history->nb_visited_states; i++) { */
/*     printf("%d  -  ",i); */
/*     for (j=0; j<parameters->feature_policy.nb_features; j++) { */
/*       printf("%i ", (int)game_history->states[i].feature_values[j]); */
/*     } */
/*     printf("    %d    %f\n",game_history->states[i].reward,game_history->states[i].state_value); */
/*   } */
/*   getchar(); */
  
/* } */


/**
 * Prints the matrices ata and the vector atb (usefull for debugging)
 */

/* static void print_matrices(LPIParameters *parameters,gsl_matrix *ata, gsl_vector *atb) { */

/*   int i,j; */
  
/*   printf("\n-------- At.A and At.b -----------\n"); */
/*   for (i = 0; i < parameters->feature_policy.nb_features; i++) { */
/*     for (j = 0; j < parameters->feature_policy.nb_features; j++) { */
/*       printf("%d ", (int) gsl_matrix_get(ata, i, j)); */
/*     } */
/*     printf("     %f\n", gsl_vector_get(atb, i)); */
/*   } */
/*   printf("\n"); */
/*   getchar(); */

/* } */

/************************ END OF DEBUG FUNCTIONS *******************/

/**
 * Initializes the algorithm.
 * This function has to be called before lambda_policy_iteration is called.
 * The parameters should have been set with ask_parameters or parse_parameters.
 */
static void initialize_lambda_policy_iteration(LPIParameters *parameters) {
  /* initialize the random number generator */
  initialize_random_generator(parameters->common_parameters.random_generator_seed);
  
  /* load the policy */
  load_feature_policy(parameters->initial_feature_file_name, &parameters->feature_policy);

  /* initialize the feature functions */
  features_initialize(&parameters->feature_policy);
}

/**
 * Frees the memory.
 */
static void exit_lambda_policy_iteration(LPIParameters *parameters) {
  exit_random_generator();
  features_exit();
  FREE(parameters->feature_policy.features);
}

/**
 * Executes the algorithm.
 * initialize_lambda_policy_iteration should have been called first.
 */
static void lambda_policy_iteration(LPIParameters *parameters) {
  GamesStatistics *games_statistics;
  GameHistory game_history;
  Game *game;
  int i, iterations;
  double max_weight_variation;

  gsl_matrix *ata; /* matrix At.A */
  gsl_vector *atb; /* vector At.b */

  iterations = 0;

  ata = gsl_matrix_alloc(parameters->feature_policy.nb_features, parameters->feature_policy.nb_features);
  atb = gsl_vector_alloc(parameters->feature_policy.nb_features);

  game_history.states = NULL;
  game_history.capacity = 0;

  game = new_game_from_parameters(&parameters->common_parameters); 

  /* initialize the statistics*/
  games_statistics = games_statistics_new(parameters->statistics_file_name, parameters->nb_games_batch, NULL);

  initialize_interruptions();
  do {
    /* play nb_games_batch games with the greedy policy with respect to the current weights */
    printf("Playing %d games...\n", parameters->nb_games_batch);
    
    gsl_matrix_set_zero(ata);
    gsl_vector_set_zero(atb);

    /* play the games */
    for (i = 0; i < parameters->nb_games_batch; i++) {
      game_reset(game);
      play_game(game, parameters, &game_history);
      printf("%d ",game->score);
      fflush(stdout);
      games_statistics_add_game(games_statistics, game->score);

    /*   game_print_history(parameters, &game_history); */

      /* update At.A and At.b */
      update_matrices(parameters, ata, atb, &game_history);
      FREE(game_history.states);
    }
    printf("\nMean score: %f\n", games_statistics->mean);

    /* print_matrices(parameters, ata, atb); */

    /* After the first episode: assign the reward, the avoid_gameover and  the 
       gameover_eval_id parameters to the first computed policy: this can be useful 
       when the parameters given to the algorithm are different from those of the initial policy */
    if (iterations==0) {
      parameters->feature_policy.reward_description.reward_function_id=parameters->common_parameters.reward_description.reward_function_id;
      parameters->feature_policy.reward_description.reward_function = parameters->common_parameters.reward_description.reward_function;
      parameters->feature_policy.gameover_evaluation = parameters->gameover_evaluation;
    }
    
    /* update the weights */
    printf("Updating the weights...\n");
    max_weight_variation = update_weights(parameters, ata, atb, iterations);

    printf("New weights: \n");
    for (i = 0; i < parameters->feature_policy.nb_features; i++) {
      printf("  %d %f\n", parameters->feature_policy.features[i].feature_id,
	     parameters->feature_policy.features[i].weight);
    }

    /* log the results */
    games_statistics_end_episode(games_statistics, &parameters->feature_policy);

    iterations++;

  } while (!is_interrupted() && iterations < parameters->nb_iterations); /* stop when Ctrl-C is pressed */

  /* save the features and the weights */
  save_feature_policy(parameters->final_feature_file_name, &parameters->feature_policy);

  /* free everything */
  gsl_matrix_free(ata);
  gsl_vector_free(atb);
  free_game(game);
  games_statistics_free(games_statistics);
}

/**
 * Plays a game and saves its history in a structure.
 * The game is played with the greedy policy with respect to a vector of weights
 * but we avoid playing a move which terminates the game. 
 */
static void play_game(Game *game, LPIParameters *parameters, GameHistory *game_history) {
  int nb_visited_states, capacity;
  int tmp;

  nb_visited_states = 0;

  /* allocate the memory to store all states */
  if (game_history->states == NULL) {
    capacity = 64;
    MALLOCN(game_history->states, StateHistory, capacity);
  }
  else { /* use the already allocated memory (from a previous game) */
    capacity = game_history->capacity;
  }
 
/*   printf("\nPlaying a new game\n"); */

  while (!game->game_over) {
    /* we have to make a move*/

    if (nb_visited_states >= capacity) {
/*       printf("reallocating the array of game states from %d to %d\n", capacity, capacity * 2); */
      capacity *= 2; /* realloc if necessary */
      REALLOC(game_history->states, StateHistory, capacity);
    }

    /* make a move */
    play_step(game, parameters, &game_history->states[nb_visited_states]);
    nb_visited_states++;

/*     game_print(stdout, game); */
/*     getchar(); */
  }
/*   printf("Game over\n"); */

  /* The Game is Over :
     Add the last term "J(I_{m,N_m},r)=J~(I_{m,N_m},r) = 0" (8.6) p. 437 */
  
  if (nb_visited_states >= capacity) {
    capacity++; 
    REALLOC(game_history->states, StateHistory, capacity);
  }
  game_history->states[nb_visited_states].feature_values = get_feature_values(game, &parameters->feature_policy);

  /* Temporarily overrides the parameter.policy.evaluate_gameover_with_features parameter 
     with  parameter.evaluate_gameover_with_features parameter 
     => useful for doing the right evaluation of the end of the game 
     (when the initial policy has a different parameter from the algorithm)  */
  tmp = parameters->feature_policy.gameover_evaluation;
  parameters->feature_policy.gameover_evaluation = parameters->gameover_evaluation;

  game_history->states[nb_visited_states].state_value = evaluate_features(game, &parameters->feature_policy); 

  parameters->feature_policy.gameover_evaluation = tmp; /* putting the original value back */

  game_history->states[nb_visited_states].reward = 0; /* <= this value will not be used because there is no subsequent state    */
  nb_visited_states++;


  game_history->nb_visited_states = nb_visited_states;
  game_history->capacity = capacity; 
}

/**
 * Makes a move using the greedy policy with respect to a vector of weights.
 * The new state is saved in a structure StateHistory.
 * @param game the game
 * @param parameters parameters of the algorithm
 * @param state_history pointer to where the new state info will be stored.
 */
static void play_step(Game *game, const LPIParameters *parameters, StateHistory *state_history) {
  Action action;

  features_get_best_action(game, &parameters->feature_policy, &action); /* use the reward function of feature_policy to choose the move */

  /* store the features and the value of current state */
  state_history->feature_values = get_feature_values(game, &parameters->feature_policy);
  state_history->state_value = evaluate_features(game, &parameters->feature_policy);

  /* play the best move found earlier (<=> go to the next state) */
  game_drop_piece(game, &action, 0);

  /* store the reward (remark: the reward is known after the move!) */
  state_history->reward = parameters->common_parameters.reward_description.reward_function(game); /* use the reward function parameter to evaluate. Remark: it can be different from the reward function used to choose the move! (this is usefull when the initial policy is based on a different criterion) */

}

/**
 * Updates the matrix At.A and the vector At.b from a game history.
 * @param parameters parameters of the algorithm
 * @param ata the matrix At.A (nb_features * nb_features)
 * @param atb the vector At.b (nb_features)
 * @param game_history history of the game
 */
static void update_matrices(const LPIParameters *parameters, gsl_matrix *ata, gsl_vector *atb,
			    GameHistory *game_history) {
  StateHistory *state, *next_state;
  double td, td_sum;
  double feature_i, feature_j, *feature_values, sum, weight = 1.0, lambdagammapn = parameters->gamma*parameters->lambda;
  int i, j, k;
  
  td_sum = 0;

  /* game_history contains the whole game 
     state[0].feature_values  state[0].state_value  state[0].reward (0->1)
     state[1].feature_values  state[1].state_value  state[1].reward (1->2)
     ...
     state[1
     

  */


  for (k = game_history->nb_visited_states - 1; k >= 0; k--) {

    /* we have to add a term to each component of the matrix At.A and the vector At.b */
    
    state = &game_history->states[k];
    feature_values = state->feature_values;
    
    if (k < game_history->nb_visited_states - 1) { /* case k<Nm (8.6) NDP p. 437 (when k=Nm the TD is 0) */
      next_state = state + 1;
      td = state->reward + parameters->gamma*next_state->state_value - state->state_value;
      td_sum = td + (parameters->lambda * parameters->gamma * (td_sum));
    }
    /* printf("r=%d vt=%f   =>  td=%f td_sum=%f  =>  b=%f\n",state->reward,state->state_value,td,td_sum, td_sum+state->state_value); */
    

    /* Biasing the samples */

    if (parameters->bias_end_of_game==1) { /* first way of weighing the samples */
      if (k < game_history->nb_visited_states - 2) { /* the first two weights are 1 */
	lambdagammapn *= parameters->lambda*parameters->gamma;
      }
      weight=(1.0-parameters->gamma*parameters->lambda)/(1.0-lambdagammapn);
    
    } else if (parameters->bias_end_of_game==2) { /* second way of weighing the samples */
      weight *= parameters->gamma*(1.0 - 1.0/game_history->nb_visited_states);
    }

    /* Update the matrix and the vector for the state */
    for (i = 0; i < parameters->feature_policy.nb_features; i++) {
      feature_i = feature_values[i];
      
      /* Update each term of the symetric matrix At.A.
       * For now, we fill only the inferior triangle of the matrix.
       */
      for (j = 0; j <= i; j++) {
	feature_j = feature_values[j];
	
	/* get the current term */
	sum = gsl_matrix_get(ata, i, j);
	sum += weight * feature_i * feature_j;
	gsl_matrix_set(ata, i, j, sum);
      }
      
      /* update the vector At.b */
      sum = gsl_vector_get(atb, i);
      sum += weight * feature_i * (state->state_value + td_sum);
      gsl_vector_set(atb, i, sum);
    }
    FREE(game_history->states[k].feature_values);    
  }
  
}
  
  /**
 * Solves the n*n linear system (At.A).x = At.b
 */
static void linear_solve(gsl_matrix *ata, gsl_vector *atb, gsl_vector *x) {
  gsl_multifit_linear_workspace *work;
  double chisq;
  gsl_matrix *cov;

  work = gsl_multifit_linear_alloc(ata->size1, x->size);
  cov = gsl_matrix_alloc(x->size, x->size);

  gsl_multifit_linear(ata, atb, x, cov, &chisq, work);

  gsl_matrix_free(cov);
  gsl_multifit_linear_free(work);
}

/**
 * Updates the weights after a batch of game was played.
 * @param parameters the parameters
 * @param ata the matrix At.A (only the inferior triangle is set)
 * @param atb the matrix At.b
 * @param nb_iterations number of LPI iterations already done
 * @return the maximum variation between two weights
 */
static double update_weights(LPIParameters *parameters, gsl_matrix *ata, gsl_vector *atb, int nb_iterations) {
  Feature *features;
  int i, j, nb_features;
  gsl_vector *x, *weight_vector;
  double sum, weight_variation, maximum_weight_variation;
  double weight, old_weight;
  double stepsize;

  maximum_weight_variation = 0;
  nb_features = parameters->feature_policy.nb_features;
  features = parameters->feature_policy.features;

  /* allocate the two weight vectors */
  weight_vector = gsl_vector_alloc(nb_features); /* current weights */
  x = gsl_vector_alloc(nb_features);             /* new weights */

  /* complete the matrix At.A */
  for (i = 0; i < nb_features; i++) {
    for (j = 0; j < i; j++) {
      sum = gsl_matrix_get(ata, i, j);
      gsl_matrix_set(ata, j, i, sum);
    }
  }

  /* print_matrices(parameters,ata,atb); */

  /* solve the least-square minimisation */
  linear_solve(ata, atb, x);

  /* x is now the new weight vector */

  if (parameters->variable_stepsize) {
    stepsize = parameters->stepsize_a / (parameters->stepsize_b + nb_iterations);
  }
  else {
    stepsize = 1;
  }

  /* update the weights in the feature list */
  for (i = 0; i < nb_features; i++) {
    old_weight = features[i].weight;
    weight = gsl_vector_get(x, i);

    weight_variation = fabs(weight - old_weight);
    maximum_weight_variation = MAX(maximum_weight_variation, weight_variation);

    features[i].weight = old_weight + stepsize * (weight - old_weight);
  }

  /* free the memory */
  gsl_vector_free(weight_vector);
  gsl_vector_free(x);

  return maximum_weight_variation;
}

/**
 * Main function.
 * Usage: ./lambda_policy_iteration [parameters]
 * 
 * Without arguments, all parameters are asked to the user.
 * If there is at least one argument, the missing parameters take their default values.
 *
 * Parameters:
 *
 * -width n                                      board width (default 10)
 * -height n                                     board height (default 20)
 * -pieces file_name                             file describing the pieces (default pieces4.dat)
 *
 * -nb_games_batch n                             number of games in an iteration (default 10)
 * -nb_iterations n                              number of iterations (default 20)
 * -lambda x                                     lambda (default 0.6)
 * -stepsize a b                                 stepsize parameter: a / (b + t) (default a=10, b=10)
 *
 * -reward {none | lines | 1 | at_least_1_line}  reward function: no reward, number of lines removed, 1 for each move,
 *                                               or 1 each time one or more lines are made
 * -initial_features file_name                   file describing the features to use and their initial weights
 * -final_features file_name                     file where the final weights will be saved
 * -statistics_file file_name                    file where the game statistics at each iteration will be saved
 */
int main(int argc, char **argv) {
  LPIParameters parameters;

  if (argc == 1) {
    ask_parameters(&parameters);
  }
  else {
    parse_parameters(argc - 1, argv + 1, &parameters);
  }

  initialize_lambda_policy_iteration(&parameters);
  lambda_policy_iteration(&parameters);
  exit_lambda_policy_iteration(&parameters);

  return 0;
}
