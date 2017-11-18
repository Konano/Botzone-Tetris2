/**
 * This program executes the Value Iteration algorithm
 * on a simplified instance of Tetris.
 */
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "config.h"
#include "simple_tetris.h"
#include "game.h"
#include "interruptions.h"
#include "macros.h"
#include "random.h"
#include "zlib.h"

/**
 * The vector of state values.
 */
static double *values;

/**
 * The vector of state values at the previous step.
 */
static double *previous_values;


/**
 * The policy in each state-piece pair (action = translation*4+orientation)
 */
static char **policy; 

/**
 * The game.
 */
static Game *game;

static ValueIterationParameters parameters;

static void value_iteration_initialise(void);
static void value_iteration(const char *value_file_name);
static void value_iteration_exit(void);
static void save_values(const char *value_file_name);
static void read_values(gzFile *value_file);

/**
 * Asks the parameters to the user and initialises the algorithm.
 */
static void value_iteration_initialise() {
  char *line;
  size_t n, read;
  int piece_set_number,i;
  FILE *delta_file;
  char c;

  /* Gamma */
  printf("Gamma (default 1): ");
  line = NULL;
  read = getline(&line, &n, stdin);
  if (read <= 1) {
    parameters.gamma = 1;
  }
  else {
    sscanf(line, "%lf", &parameters.gamma);
  }

  /* Use a buffer? */
  printf("Use a buffer (y/n, default y): ");
  FREE(line);
  line = NULL;
  read = getline(&line, &n, stdin);
  if (read <= 1) {
    c = 'y';
  }
  else {
    sscanf(line, "%c", &c);
  }
  if (c == 'n') {
    parameters.use_buffer = 0;
  }
  else {
    parameters.use_buffer = 1;
  }

  /* Limit for delta */
  printf("Delta limit (default 0): ");
  FREE(line);
  line = NULL;
  read = getline(&line, &n, stdin);
  if (read <= 1) {
    parameters.delta_limit = 0;
  }
  else {
    sscanf(line, "%lf", &parameters.delta_limit);
  }

  /* Piece set */
  printf("Set of pieces (default 1) :\n");
  printf("  1. Standard Tetris pieces (tetraminos)\n");
  printf("  2. Melax's reduced set of pieces\n");
  printf("  Your choice: ");
  FREE(line);
  line = NULL;
  read = getline(&line, &n, stdin);
  
  if (read <= 1) {
    piece_set_number = 1;
  }
  else {
    sscanf(line, "%d", &piece_set_number);
  }

  if (piece_set_number == 2) {
    strcpy(parameters.piece_file_name, "pieces_melax.dat");
  }
  else {
    strcpy(parameters.piece_file_name, "pieces4.dat");
  }

  /* Delta history file */
  printf("File to save the value of delta at each iteration (default delta.dat): ");
  FREE(line);
  line = NULL;
  read = getline(&line, &n, stdin);

  if (read <= 1 || read >= MAX_FILE_NAME) {
    strcpy(parameters.delta_file_name, "delta.dat");
  }
  else {
    line[read-1] = '\0';
    strcpy(parameters.delta_file_name, line);
  }
  FREE(line);
  delta_file = fopen(parameters.delta_file_name, "w");
  fprintf(delta_file, "# History of delta during Value Iteration.\n# Parameters: gamma = %f, delta_limit = %f\n", parameters.gamma, parameters.delta_limit);
  fclose(delta_file);
  
  /* Number of iterations already done */
  parameters.iterations = 0;

  /* Make the board */
  game = new_game(0, WIDTH, HEIGHT, 0, parameters.piece_file_name, NULL);
  parameters.nb_pieces = game->piece_configuration->nb_pieces;

  /* Allocate the values */
  CALLOC(values, double, NB_STATES);

  if (parameters.use_buffer) {
    CALLOC(previous_values, double, NB_STATES);
  }

  /* Allocate the policy */
  CALLOC(policy, char*, NB_STATES);
  for (i=0; i<NB_STATES; i++) {
    CALLOC(policy[i],char,parameters.nb_pieces);
  }

}

/**
 * Frees the memory.
 */
static void value_iteration_exit() {

  FREE(values);
  if (parameters.use_buffer) {
    FREE(previous_values);
  }
  free_game(game);

}

/**
 * Executes the algorithm Value Iteration.
 */
static void value_iteration(const char *value_file_name) {
  /* the state */
  uint32_t s;               /* current state */
  double v;                 /* current value of state s, taking action (i,j) */
  int nb_pieces;            /* number of possible pieces */

  /* the action: a couple (orientation, column) */
  int nb_possible_columns, nb_possible_orientations;
  int i;                    /* orientation of the falling piece */
  int j;                    /* column of the falling piece */
  double action_value;      /* value of state s, taking action (i,j) */
  double best_action_value; /* value of state s, taking the best action */
  int k;                    /* number of a possible next falling piece after s */
  uint32_t s2;              /* one possible state after s, taking action (i,j) */
  int reward;               /* immediate reward when taking action (i,j) in state s */

  /* algorithm convergence */
  double gamma;             /* discount factor */
  int iterations;           /* number of iterations */
  double delta;             /* maximum change of a state's value during an iteration */
  double delta_limit;       /* limit of delta to stop the algorithm */
  double value_change;      /* change of a state's value during an iteration */
  FILE *delta_file;         /* file to save the value of delta at each iteration */
  double *tmp_values;       /* variable to swap the buffers */
  char best_action_code;    /* best_action_code = 4*column + orientation */
  int nb_changed_actions;   /* count the number of actions that change from one iteration to the next */

  Board *board;
  PieceOrientation *orientations;

  char file_name[256];

  /* stop the algorithm when the user presses Ctrl-C */
  initialize_interruptions();

  /* start Value Iteration */
  delta_file = fopen(parameters.delta_file_name, "a");

  board = game->board;
  nb_pieces = parameters.nb_pieces;
  gamma = parameters.gamma;
  delta_limit = parameters.delta_limit;
  iterations = parameters.iterations;

  if (!parameters.use_buffer) {
    /* if we don't use a buffer, values and previous_values are the same pointer */
    previous_values = values;
  }

  do {
    delta = 0;
    
    nb_changed_actions=0;
    best_action_code=0;

    /* visit each state to update its value */
    for (s = 0; s < NB_STATES; s++) { 
      set_game_state(game, s);
      v = previous_values[s]; /* current value of the state */

/*       if (s == 0) { */
/* 	printf("--------------\nevaluating state %d\n", s); */
/* 	game_print(stdout, game); */
/* 	printf("current value of this state: %f\n", v); */
/*       } */

      /* We have to make a sum of values for each possible current piece in state s */
      values[s] = 0;
      for (k = 0; k < nb_pieces; k++) { /* consider each possible piece */
	game_set_current_piece_index(game, k);
/* 	if (s == 0) printf("current piece: %d\n", k); */

	/* Now we are in state s with piece k as current piece.
	 * We try every action and keep the best one's value.
	 */

	best_action_value = -1;
	orientations = game->current_piece->orientations;
	nb_possible_orientations = game_get_nb_possible_orientations(game);

	for (i = 0; i < nb_possible_orientations; i++) {
	  nb_possible_columns = game_get_nb_possible_columns(game, i);
	  for (j = 1; j <= nb_possible_columns; j++) {
/* 	    if (s == 0) printf("Action: orientation %d, column %d\n", i, j); */
	    
	    /* At this point we have selected an action.
	     * Now we play this action to evaluate it.
	     */
	    reward = board_drop_piece(board, &orientations[i], i, j, NULL, 1); /* immediate reward */	    
	    if (game->board->wall_height > game->board->height) {
	      reward=-1;
	    } else {
	      s2 = get_game_code(game); /* get the id of the resulting state */
	    }
	    board_cancel_last_move(board); /* restore the board */
	    
	    action_value = 0; /* value of the action we made */
	    if (reward != -1) { /* not game over */	      
	      action_value = reward + gamma * previous_values[s2];
	    }
/* 	    if (s == 0) printf("in state %d: reward = %d, new action value = %f\n", s2, reward, action_value); */

	    /* if we found a better value for this action, keep it */
	    if (action_value>best_action_value) {
	      best_action_value = action_value;
	      best_action_code = 4*j+i;
	    }
	  }
	}	
	values[s] += best_action_value;

	if (policy[s][k] != best_action_code) {
	  nb_changed_actions++;
	  policy[s][k]=best_action_code;
	}


/* 	if (s == 0) printf("value of this state with piece %d: %f\n", k, best_action_value); */
      }
/*       if (s == 0) printf("total value of this state: %f\n", values[s]); */
      values[s] /= nb_pieces; /* normalization */

      value_change = fabs(v - values[s]);
      delta = MAX(delta, value_change);

      if (s % 100000 == 0) { /* just to print the progression */
	printf("  state %d\r", s);
	fflush(stdout);
      }
    }

    if (parameters.use_buffer) {
      tmp_values = previous_values;
      previous_values = values;
      values = tmp_values;
    }

    printf("%d iteration(s), delta v = %e, delta pi = %d\n", ++iterations, delta, nb_changed_actions);
    fprintf(delta_file, "%d %e %d\n", iterations, delta,nb_changed_actions);
    fflush(delta_file);

    /* save the current values */
    sprintf(file_name,"%s.%05i",value_file_name,iterations);
    parameters.iterations = iterations;
    save_values(file_name);

    /* stop when delta reaches the limit or when the user has pressed Ctrl-C */
  } while (delta > delta_limit && !is_interrupted());
  fclose(delta_file);

  /* save the values */
  parameters.iterations = iterations;
  save_values(value_file_name);
}

/**
 * Saves the state values into a binary file.
 */
static void save_values(const char *value_file_name) {
  gzFile *value_file;

  printf("Saving the state values into the file %s... ",value_file_name);
  value_file = gzopen(value_file_name, "w");
  gzwrite(value_file, &parameters, sizeof(ValueIterationParameters));
  gzwrite(value_file, values, sizeof(double)*NB_STATES);

  /*if (parameters.use_buffer) {
    fwrite(previous_values, sizeof(double), NB_STATES, value_file);
    }*/

  gzclose(value_file);
  printf("Done.\n");
}

/**
 * Initializes the algorithm with some existing parameters and state values
 * from a file.
 */
static void read_values(gzFile *value_file) {

  gzread(value_file, &parameters, sizeof(ValueIterationParameters));

  MALLOCN(values, double, NB_STATES);

  if (parameters.use_buffer == 1) {
    MALLOCN(previous_values, double, NB_STATES);
    gzread(value_file, previous_values, sizeof(double)*NB_STATES);
  } else {
    gzread(value_file, values, sizeof(double)*NB_STATES);
  }

  game = new_game(0, WIDTH, HEIGHT, 0, parameters.piece_file_name, NULL);

  printf("Resuming value iteration with the following parameters:\n");
  printf("nb_pieces: %d, use_buffer: %d, delta_limit: %f, iterations: %d, piece_file_name: %s, delta_file_name: %s\n",
	 parameters.nb_pieces, parameters.use_buffer, parameters.delta_limit, parameters.iterations,
	 parameters.piece_file_name, parameters.delta_file_name);
}

/**
 * Main function.
 * Usage: ./value_iteration value_file_name
 */
int main(int argc, char **argv) {
  gzFile *value_file;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s value_file_name\n", argv[0]);
    exit(1);
  }
  
  initialize_random_generator(time(NULL));

  value_file = gzopen(argv[1], "r");
  if (value_file == NULL) {
    /* new instance of value iteration */
    value_iteration_initialise();
  }
  else {
    /* continue an existing execution */
    read_values(value_file);
  }
  
  value_iteration(argv[1]);
  value_iteration_exit();

  exit_random_generator();

  return 0;
}
