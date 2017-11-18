/**
 * A simple executable to play some games with a given feature policy file.
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "config.h"
#include "random.h"
#include "play_games.h"


static void print_usage(int argc, char **argv);

/**
 * Prints the command line syntax.
 */
static void print_usage(int argc, char **argv) {
  fprintf(stderr, "Usage: %s feature_file nb_games board_width board_height statistics_file [comments]\n", argv[0]);
}


/**
 * @brief Do an epsiode of nb_games games, update the GameStatisctics object
 * @param game an initialized Game object
 * @param strategy the strategy to be used
 * @param nb_games the number of games to play
 * @param stats an initialized structure for storing the statistics
 */
void play_games(Game *game, Strategy *strategy, int nb_games, GamesStatistics *stats) {

  int i,score;  

  /* play the games */
  for (i = 0; i < nb_games; i++) {
    game_reset(game);
    score = strategy_play_game(strategy, game);
    printf("%d ", score);
    fflush(stdout);
    games_statistics_add_game(stats, score);
  }
  printf("\nMean score: %f\n", stats->mean);

}


/**
 * Main function.
 * Usage: mdptetris_play_games feature_file nb_games board_width board_height statistics_file [comments]
 */
int main(int argc, char **argv) {
  int nb_games, board_width, board_height;
  char *feature_file_name, *statistics_file_name, *comments;
  Strategy *strategy;
  GamesStatistics *stats;
  Game *game;

  /* parse the command line */
  if (argc != 6 && argc != 7) {
    print_usage(argc, argv);
    exit(1);
  }

  feature_file_name = argv[1];
  nb_games = atoi(argv[2]);
  board_width = atoi(argv[3]);
  board_height = atoi(argv[4]);
  statistics_file_name = argv[5];
  comments = (argc == 7) ? argv[6] : NULL;

  /* initialize everything */
  initialize_random_generator(time(NULL));
  strategy = new_strategy_fixed_weights(feature_file_name);
  stats = games_statistics_new(statistics_file_name, nb_games, comments);
  game = new_game(0, board_width, board_height, 0, "pieces4.dat", NULL);
  strategy->initialize(game);

  /* play the games */
  play_games(game, strategy, nb_games, stats);

  /* free everything */
  strategy->exit();
  games_statistics_end_episode(stats, NULL);
  games_statistics_free(stats);
  exit_random_generator();
  printf("Statistics saved in file '%s'.\n", statistics_file_name);

  return 0;
}
