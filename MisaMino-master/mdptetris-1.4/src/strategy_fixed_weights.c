#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "strategy_fixed_weights.h"
#include "game.h"
#include "rewards.h"
#include "feature_policy.h"
#include "feature_functions.h"
#include "macros.h"
#include "file_tools.h"

static void decide(Game *game, Action *best_action);
static void initialize(Game *game);
static double strategy_print_features(Game *game);

static char *feature_file_name = NULL;
static FeaturePolicy feature_policy;

/**
 * This function is called before a game or a set of game is played.
 * @param game the game that will be played
 */
static void initialize(Game *game) {

  /* ask the file if the feature file was not specified */
  if (feature_file_name == NULL) {
    MALLOCN(feature_file_name, char, MAX_FILE_NAME);
    printf("Name of the feature file: ");
    SCANF("%s", feature_file_name);
    while (getchar() != '\n');
  }

  /* get the features from the file */
  load_feature_policy(feature_file_name, &feature_policy);

  /* initialize the feature functions */
  features_initialize(&feature_policy);
}

/**
 * This function is called after the games are played.
 */
static void strategy_exit(void) {
  features_exit();
  FREE(feature_policy.features);
}

/**
 * Tries all possible moves, evaluate them and selects the best one.
 * @param game the game
 * @param action pointer to where the chosen action will be stored
 */
static void decide(Game *game, Action *action) {
  features_get_best_action(game, &feature_policy, action);
}

/**
 * Displays the values of the features (usefull for debugging/watching).
 */
static double strategy_print_features(Game *game) {
  print_features(&feature_policy);
  game_print_features(game, &feature_policy);
  return(evaluate_features(game, &feature_policy));
}

/**
 * Creates a new strategy with fixed weights.
 * @param feature_file name of the feature file (can be NULL, then
 * the file name will be asked to the user)
 */
Strategy *new_strategy_fixed_weights(char *feature_file) {
  Strategy *strategy;

  MALLOC(strategy, Strategy);

  if (feature_file != NULL) {
    MALLOCN(feature_file_name, char, strlen(feature_file) + 1);
    strcpy(feature_file_name, feature_file);
  } else {
    if (feature_file_name != NULL) {
      FREE(feature_file_name);
    }
  }

  strategy->initialize = initialize;
  strategy->exit = strategy_exit;
  strategy->decide = decide;
  strategy->info = strategy_print_features;
  return strategy;
}
