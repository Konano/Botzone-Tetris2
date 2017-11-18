/**
 * A simple executable to play some moves with a given feature policy file,
 * and to estimate the probability of losing from this sample of moves.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include "config.h"
#include "estimate_duration.h"
#include "feature_functions.h"
#include "random.h"

static void print_usage(int argc, char **argv);

/**
 * Prints the command line syntax.
 */
static void print_usage(int argc, char **argv) {
  fprintf(stderr, "Usage: %s feature_file nb_moves board_width board_height\n", argv[0]);
}


/**
 * Main function.
 * Usage: mdptetris_estimate_distribution feature_file nb_moves board_width board_height
 */
int main(int argc, char **argv) {

  int nb_moves, board_width, board_height;
  char *feature_file_name;
  FeaturePolicy feature_policy;
  Game *game;
  double duration, score;

  /* parse the command line */
  if (argc != 5) {
    print_usage(argc, argv);
    exit(1);
  }

  feature_file_name = argv[1];
  nb_moves = atoi(argv[2]);
  board_width = atoi(argv[3]);
  board_height = atoi(argv[4]);

  /* initialize everything */
  initialize_random_generator(times(NULL));

  load_feature_policy(feature_file_name, &feature_policy);
  features_initialize(&feature_policy);

  game = new_game(0, board_width, board_height, 0, "pieces4.dat", NULL);

  /* play the moves and make the estimation */
  duration = play_moves_estimate_duration(game, &feature_policy, nb_moves, stdout);
  score = 4 * duration / board_width;

  printf("# Mean game duration: %f (mean score: %f)\n", duration, score);

  /* free everything */
  features_exit();
  exit_random_generator();

  return 0;
}
