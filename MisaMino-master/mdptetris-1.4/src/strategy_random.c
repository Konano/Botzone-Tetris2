#include <stdlib.h>
#include "config.h"
#include "types.h"
#include "strategy_random.h"
#include "game.h"
#include "random.h"
#include "macros.h"

static void decide(Game *game, Action *best_action);

/**
 * Chooses a random orientation and a random column for the current piece.
 * @param game the game
 * @param best_action pointer to where the chosen action will be stored
 */
static void decide(Game *game, Action *best_action) {
  best_action->orientation = random_uniform(0, game_get_nb_possible_orientations(game));
  best_action->column = random_uniform(0, game_get_nb_possible_columns(game, best_action->orientation)) + 1;
}

/**
 * Creates a new random strategy.
 * With this very poor strategy, each move is chosen randomly.
 */
Strategy *new_strategy_random() {
  Strategy *strategy;

  MALLOC(strategy, Strategy);
  strategy->initialize = NULL;
  strategy->exit = NULL;
  strategy->decide = decide;
  strategy->info = NULL;
  return strategy;
}
