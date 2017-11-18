/**
 * Strategy to play a simple tetris game with the
 * optimal policy computed by Value Iteration.
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "types.h"
#include "strategy_value_iteration.h"
#include "game.h"
#include "macros.h"
#include "simple_tetris.h"
#include "zlib.h"

static ValueIterationParameters parameters;

/**
 * The matrix of state values.
 */
static double values[NB_STATES];


static void decide(Game *game, Action *best_action);
static void initialize(Game *game);
static double print_value(Game *game);

/**
 * This function is called before a game or a set of game is played.
 * @param game the game that will be played
 */
static void initialize(Game *game) {
  char value_file_name[MAX_FILE_NAME];
  gzFile *value_file;

  /* check that the game configuration is ok */
  if (game->board->width < 4 || game->board->width > 5 || game->board->height != 5 || game->piece_configuration->nb_pieces > 8) {
    DIE("Cannot initialise the strategy value iteration for simple tetris.\nInvalid game configuration: The board dimensions should be 4*5 or 5*5 and the piece set should not exceed 8 elements.");
  }
  
  /* read the values */
  printf("Name of the value file: ");
  SCANF("%s", value_file_name);
  while (getchar() != '\n');

  value_file = gzopen(value_file_name, "r");

  if (value_file == NULL) {
    DIE("Unable to open the value file\n");
  }

  gzread(value_file, &parameters, sizeof(ValueIterationParameters));
  gzread(value_file, values, sizeof(double)*NB_STATES);
  gzclose(value_file);
}

/**
 * Tries all possible moves, evaluate their state values and selects the best one.
 * @param game the game
 * @param best_action pointer to where the chosen action will be stored
 */
static void decide(Game *game, Action *best_action) {
  int nb_possible_orientations, nb_possible_columns, i, j, removed_lines;
  double rating, best_rating;
  Action action;

/*   printf("-----------\nI have to decide in the following situation:\n"); */
/*   game_print(stdout, game); */
  
  best_rating = -TETRIS_INFINITE;
  best_action->orientation = 0;
  best_action->column = 1;
  
  nb_possible_orientations = game_get_nb_possible_orientations(game);
  for (i = 0; i < nb_possible_orientations; i++) {
    action.orientation = i;
    nb_possible_columns = game_get_nb_possible_columns(game, i);
    for (j = 1; j <= nb_possible_columns; j++) {
      action.column = j;

 /*      printf("  Evaluating an action: orientation %d, column %d\n", i, j); */

      removed_lines = game_drop_piece(game, &action, 1);

 /*      printf("  Resulting state:\n"); */
/*       game_print(stdout, game); */
/*       getchar(); */

      if (game->game_over) {
      	rating = -TETRIS_INFINITE;
      }
      else {
	rating = removed_lines + parameters.gamma * values[get_game_code(game)];
      }
/*       printf("  Rating: %f\n", rating); */

      if (rating > best_rating) {
	best_rating = rating;
	best_action->orientation = i;
	best_action->column = j;
      }
      game_cancel_last_move(game);
    }
  }
/*   printf("Decision made\n"); */
}


/**
 * Prints the value of the current game state
 */
static double print_value(Game *game) {
  
  double value=values[get_game_code(game)];

  printf("Value = %f\n",value);
  return(value);

}



/**
 * Creates a new Value Iteration strategy.
 */
Strategy *new_strategy_value_iteration(void) {
  Strategy *strategy;

  /* allocate the strategy */
  MALLOC(strategy, Strategy);
  strategy->initialize = initialize;
  strategy->exit = NULL;
  strategy->decide = decide;
  strategy->info = print_value;

  return strategy;
}
