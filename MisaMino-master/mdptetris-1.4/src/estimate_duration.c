/**
 * A simple executable to play some moves with a given feature policy file,
 * and to estimate the probability of losing from this sample of moves.
 */

#include <stdlib.h>
#include <gsl/gsl_fit.h>
#include "config.h"
#include "estimate_duration.h"
#include "feature_policy.h"
#include "macros.h"

/**
 * @brief Plays moves with some strategy and estimates the probability of reaching each wall height.
 *
 * The number of moves specified are played, in one or several games.
 * During the game(s), we count the number of times each wall height is reached.
 * Then, a fitting is made to estimate the probability of reaching the game over height.
 * The value returned is the estimated mean game duration, i.e. the inverse of this probability.
 * Thus, the game duration can be estimated precisely even if there was no game over during the moves.
 *
 * Note that the estimated duration is lower than the real mean duration when the strategy
 * avoids game over actions or when the strategy gives a special value to the game over states.
 * However, even in such cases, the value returned is a good heuristic to compare different policies.
 *
 * @param game an initialized Game object
 * @param feature_policy the policy to be used
 * @param nb_moves the number of moves to play (if the game is over before the
 * number of moves are played, a new game is played)
 * @param out a file to print the frequencies (then you can gnuplot them), can be NULL
 * @return an estimation of the game duration, i.e. the mean number of moves before the game is over.
 */
double play_moves_estimate_duration(Game *game, const FeaturePolicy *feature_policy, int nb_moves, FILE *out) {

#define FIT_FIRST_HEIGHT 7
#define FIT_LAST_HEIGHT 15

  int i;
  Action action;
  Board *board;
  int max_height, fit_nb_heights;

  int *heights_visited;    /* number of times each height is visited */
  double *heights;         /* list of the heights (x axis for the linear regression) */
  double *frequencies;     /* proportion of visit for each height */
  double *log_frequencies; /* log of the visit frequency of each height (y axis for the linear regression) */

  double a, b, cov00, cov01, cov11, sumq;
  double gameover_proba;

  board = game->board;

  /* TODO: don't allocate memory each time */
  CALLOC(heights_visited, int, board->height + 1);
  CALLOC(heights, double, board->height + 1);
  CALLOC(frequencies, double, board->height + 1);
  CALLOC(log_frequencies, double, board->height + 1);

  /* play the moves */  
  for (i = 0; i < nb_moves; i++) {

    features_get_best_action(game, feature_policy, &action);
    game_drop_piece(game, &action, 0);

    heights_visited[board->wall_height]++;

    if (game->game_over) {
      game_reset(game);
    }
  }

  max_height = 0;

  for (i = 0; i <= board->height; i++) {
    heights[i] = i;
    frequencies[i] = ((double) heights_visited[i]) / ((double) nb_moves);
    if (frequencies[i] > 0) {
      log_frequencies[i] = log(frequencies[i]);

      if (out != NULL) {
	fprintf(out, "%d %f\t# visited %d times (frequency = %f)\n", i,
		log_frequencies[i], heights_visited[i], frequencies[i]);
      }

      max_height = i;
    }
  }

  fit_nb_heights = MIN(FIT_LAST_HEIGHT, max_height) - FIT_FIRST_HEIGHT + 1;

  if (fit_nb_heights < 2) {
    DIE("The board height is too low to make the fitting\n");
  }

  /* gsl fit */
  gsl_fit_linear(heights + FIT_FIRST_HEIGHT, 1,
		 log_frequencies + FIT_FIRST_HEIGHT, 1,
		 fit_nb_heights,
		 &b, &a, &cov00, &cov01, &cov11, &sumq);

  /* now a*x + b is the best fit line */
  if (out != NULL) {
    fprintf(out, "# best fit: y = %f * x + %f\n", a, b);
  }

  FREE(heights_visited);
  FREE(heights);
  FREE(frequencies);
  FREE(log_frequencies);

  gameover_proba = exp(a * (board->height + 1) + b);

  return 1.0 / gameover_proba;
}

