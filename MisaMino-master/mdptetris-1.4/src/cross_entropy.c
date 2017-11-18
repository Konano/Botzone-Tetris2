#include <math.h>
#include <gsl/gsl_sort_double.h>
#include "config.h"
#include "cross_entropy_parameters.h"
#include "feature_functions.h"
#include "games_statistics.h"
#include "estimate_duration.h"
#include "interruptions.h"
#include "random.h"

static void initialize_cross_entropy(CrossEntropyParameters *parameters);
static void exit_cross_entropy(CrossEntropyParameters *parameters);
static void cross_entropy(CrossEntropyParameters *parameters);
static void save_current_features(CrossEntropyParameters *parameters);

static void generate_feature_sample(const CrossEntropyParameters *parameters, FeaturePolicy *feature_policies);
static double evaluate_feature_sample(const CrossEntropyParameters *parameters, const FeaturePolicy *feature_policy, Game *game);

static void update_distribution(CrossEntropyParameters *parameters, FeaturePolicy *feature_policies,
				double *evaluations, int nb_vectors_kept, size_t *best_vector_indices,
				int episode_number);

/**
 * Initializes the algorithm.
 * This function has to be called before cross_entropy() is called.
 * The parameters should have been set with ask_parameters() or parse_parameters().
 */
static void initialize_cross_entropy(CrossEntropyParameters *parameters) {
  /* initialize the random number generator */
  initialize_random_generator(parameters->common_parameters.random_generator_seed);
  
  /* initialize the features */
  cross_entropy_load_feature_policy(parameters);
  features_initialize(&parameters->feature_policy);
}

/**
 * Frees the memory.
 */
static void exit_cross_entropy(CrossEntropyParameters *parameters) {
  exit_random_generator();
  FREE(parameters->means);
  FREE(parameters->variances);
  FREE(parameters->feature_policy.features);
}

/**
 * Executes the algorithm.
 * initialize_cross_entropy() should have been called first.
 */
static void cross_entropy(CrossEntropyParameters *parameters) {
  int i, nb_features, nb_episodes, nb_vectors_kept;
  Game *game, *game_evaluating;
  FeaturePolicy *feature_policies;
  GamesStatistics *games_statistics;
  double *evaluations, mean_performance, performance, best_performance;
  size_t *best_vector_indices;

  /* allocate memory */
  nb_features = parameters->feature_policy.nb_features;
  nb_vectors_kept = (int) (parameters->nb_vectors_generated * parameters->rho);

  CALLOC(feature_policies, FeaturePolicy, parameters->nb_vectors_generated);
  MALLOCN(evaluations, double, parameters->nb_vectors_generated);
  MALLOCN(best_vector_indices, size_t, nb_vectors_kept);

  game = new_game_from_parameters(&parameters->common_parameters);
  game_evaluating = new_game(parameters->common_parameters.tetris_implementation, 
			     parameters->common_parameters.board_width,
			     parameters->board_height_when_evaluating,
			     parameters->common_parameters.allow_lines_after_overflow,
			     parameters->common_parameters.piece_file_name,
			     NULL);

  /* initialize Ctrl-C handling */
  initialize_interruptions();

  /* initialize the statistics */
  games_statistics = games_statistics_new(parameters->statistics_file_name,
					  parameters->nb_games_after_update, NULL);
  
  nb_episodes = games_statistics->nb_episodes; /* number of episodes already done */
  best_performance = games_statistics->best_mean; /* best mean of the 30 games played after an episode */

  do {
    /* generate the vectors of features */
    printf("Generating %d feature vectors\n", parameters->nb_vectors_generated);
    for (i = 0; i < parameters->nb_vectors_generated; i++) {
      generate_feature_sample(parameters, &feature_policies[i]);
    }
    
    /* evaluate each vector */

    printf("Evaluating each vector, playing %d games on a %d*%d board\n", parameters->nb_evaluating_games, parameters->common_parameters.board_width, parameters->board_height_when_evaluating);

    /* Fahey's method
    printf("Evaluating each vector, playing %d moves on a %d*%d board\n", 100000, parameters->common_parameters.board_width, parameters->board_height_when_evaluating);
    */

    mean_performance = 0;
    for (i = 0; i < parameters->nb_vectors_generated; i++) {
      evaluations[i] = evaluate_feature_sample(parameters, &feature_policies[i], game_evaluating);
      mean_performance += evaluations[i];
    }
    mean_performance /= parameters->nb_vectors_generated;
    printf("\nMean performance of the generated vectors: %f\n", mean_performance);
    
    /* keep the rho * n best vectors */
    gsl_sort_largest_index(best_vector_indices, nb_vectors_kept, evaluations, 1, parameters->nb_vectors_generated);

    printf("keeping %d vectors:", nb_vectors_kept);
    for (i = 0; i < nb_vectors_kept; i++) {
      printf(" %f",  evaluations[best_vector_indices[i]]);
    }
    printf("\n");

    printf("Features of the best vector:\n");
    print_features(&feature_policies[best_vector_indices[0]]);

    /* update mu and sigma2 */
    printf("Updating the distribution\n");
    update_distribution(parameters, feature_policies, evaluations, nb_vectors_kept, best_vector_indices, nb_episodes);

    /* After the first episode: assign the reward and the 
       gameover_evaluation parameters to the first computed policy: this can be useful 
       when the parameters given to the algorithm are different from those of the initial policy */
    if (nb_episodes == 0) {
      parameters->feature_policy.reward_description = parameters->common_parameters.reward_description;
      parameters->feature_policy.gameover_evaluation = -1;
    }

    /* old way: play nb_games_after_update games */
    printf("Playing %d games with the mean feature weights on a %d*%d board \n",
	   parameters->nb_games_after_update, parameters->common_parameters.board_width,
	   parameters->common_parameters.board_height); 

    performance = feature_policy_play_games(&parameters->feature_policy, parameters->nb_games_after_update,
					    game, games_statistics, 1);
 
    printf("\nMean score: %f\n", performance);
    
    /* other way: estimate the performance with Fahey's method
    printf("Estimating the performance of the mean weight policy\n");
    performance = play_moves_estimate_duration(game, &parameters->feature_policy, 1000000, NULL);
    games_statistics_add_game(games_statistics, performance);
    games_statistics_end_episode(games_statistics, &parameters->feature_policy);
    printf("Estimation = %f\n", performance);
    */

    /* save the feature policy if it is the best one ever seen */
    if (performance > best_performance) {
      best_performance = performance;
      save_feature_policy(parameters->final_feature_file_name, &parameters->feature_policy);
    }

    /* save the current feature distributions anyway in the current feature file
     * (useful if the execution is stopped and resumed later)
     */
    save_current_features(parameters);

    nb_episodes++;

  } while (!is_interrupted() && nb_episodes < parameters->nb_episodes);
  
  /* free everything */
  games_statistics_free(games_statistics);
  free_game(game);
  free_game(game_evaluating);
  FREE(evaluations);
  FREE(best_vector_indices);

  for (i = 0; i < parameters->nb_vectors_generated; i++) {
    FREE(feature_policies[i].features);
  }
  FREE(feature_policies);
}

/**
 * Saves in a file the current mean and variance of each feature.
 * The file name is given by parameters->current_features_file_name.
 * This is useful to resume the execution later if it is stopped.
 * To resume the execution later, rerun the program with this file as initial feature file.
 */
static void save_current_features(CrossEntropyParameters *parameters) {
  FILE *current_feature_file;
  FeaturePolicy *feature_policy;
  int i;

  feature_policy = &parameters->feature_policy;
  current_feature_file = fopen(parameters->current_feature_file_name, "w");

  if (current_feature_file == NULL) {
    DIE("Unable to write the current feature file");
  }

  /* write the reward function id */
  fprintf(current_feature_file, "%d\n", feature_policy->reward_description.reward_function_id);

  /* write the gameover evaluation value */
  fprintf(current_feature_file, "%d\n", feature_policy->gameover_evaluation);

  /* write the number of features */
  fprintf(current_feature_file, "%d\n", feature_policy->nb_features);

  /* write each feature, its mean and its variance */
  for (i = 0; i < feature_policy->nb_features; i++) {
    fprintf(current_feature_file, "%d %e %e\n", feature_policy->features[i].feature_id,
	    parameters->means[i], parameters->variances[i]);
  }

  fclose(current_feature_file);
}

/**
 * Generates a feature vector. Each feature i has a distribution N(mu(i)), sigma2(i)) where mu(i) and sigma2(i))
 * are stored in the parameters.
 */
static void generate_feature_sample(const CrossEntropyParameters *parameters, FeaturePolicy *feature_policy) {
  int i, nb_features;
  Feature *features;
  
  nb_features = parameters->feature_policy.nb_features;

  features = feature_policy->features;
  if (features == NULL) {
    /* first iteration: initialize the feature policy
     * during the next iterations, only the weight will be updated
     */
    MALLOCN(features, Feature, nb_features);
    feature_policy->nb_features = nb_features;
    feature_policy->update_column_heights_needed = parameters->feature_policy.update_column_heights_needed;
    feature_policy->reward_description = parameters->common_parameters.reward_description;
    feature_policy->gameover_evaluation = 1;

    for (i = 0; i < nb_features; i++) {
      features[i] = parameters->feature_policy.features[i];
    }
  }

  for (i = 0; i < nb_features; i++) {
    features[i].weight = random_gaussian(parameters->means[i], sqrt(parameters->variances[i]));
  }

  feature_policy->features = features;
}

/**
 * Plays games with a specific feature vector to evaluate it.
 */
static double evaluate_feature_sample(const CrossEntropyParameters *parameters, const FeaturePolicy *feature_policy, Game *game) {

  double evaluation;

  /* old way: play k games */
  evaluation = feature_policy_play_games(feature_policy, parameters->nb_evaluating_games, game, NULL, 0);

  /* other way: estimate the game duration with Fahey's method
  evaluation = play_moves_estimate_duration(game, feature_policy, 100000, NULL);
  */

  printf("%f ", evaluation);
  fflush(stdout);

  return evaluation;
}

/**
 * Updates mu and sigma2 according to the best rho * n feature vectors generated.
 */
static void update_distribution(CrossEntropyParameters *parameters, FeaturePolicy *feature_policies,
				double *evaluations, int nb_vectors_kept, size_t *best_vector_indices,
				int episode_number) {
  int i, j, index, nb_features;
  double mu, sigma2, feature_value;

  nb_features = parameters->feature_policy.nb_features;

  /* compute mu and sigma */
  for (i = 0; i < nb_features; i++) {

    mu = 0; /* mu is the mean */
    sigma2 = 0; /* sigma2 will be the variance */

    for (j = 0; j < nb_vectors_kept; j++) {
      index = best_vector_indices[j];
      feature_value = feature_policies[index].features[i].weight;
      mu += feature_value;
      sigma2 += feature_value*feature_value;
    }

    mu /= nb_vectors_kept;
    parameters->means[i] = mu;
    parameters->feature_policy.features[i].weight = mu;
    
    sigma2 /= nb_vectors_kept;  
    sigma2 -= mu*mu;             /* E[(X - E[X])^2 = E[X^2]-E[X]^2 */
    sigma2 = MAX(sigma2, 0.0);
/*     fprintf(stderr, "Applying noise %f\n", parameters->noise_function(&parameters->noise_parameters, episode_number)); */
    parameters->variances[i] = sigma2 + parameters->noise_function(&parameters->noise_parameters, episode_number);
  }

  /* print the new distribution */
  printf("New mu and sigma2:\n");
  for (i = 0; i < nb_features; i++) {
    printf("%d: mu = %f, sigma2 = %f\n", parameters->feature_policy.features[i].feature_id,
	   parameters->means[i], parameters->variances[i]);
  }
}

/**
 * Main function.
 * Usage: ./cross_entropy [parameters]
 * 
 * Without arguments, all parameters are asked to the user.
 * If there is at least one argument, the missing parameters take their default values.
 *
 * Parameters:
 *
 * -width n                                      board width (default 10)
 * -height n                                     board height (default 20)
 * -pieces file_name                             file describing the pieces (default pieces4.dat)
 * -reward {none | lines | 1 | at_least_1_line}  reward function: no reward, number of lines removed, 1 for each move,
 *                                               or 1 each time one or more lines are made (default lines)
 *
 * -initial_features file_name                   file describing the features and the initial distributions
 *                                               of their weights
 *
 * -final_features file_name                     file where the final weights will be saved (default
 *                                               features_bertsekas.dat
 * -statistics_file file_name                    file where the game statistics at each iteration will be saved
 *                                               (default ce_statistics.dat)
 * -nb_vectors_generated n                       number of sample feature vectors to generate at each iteration
 *                                               (default 100)
 * -nb_evaluating_games n                        number of games played to evaluate a vector generated (default 1)
 * -nb_games_after_update n                      number of games to play with the mean weights after a
 *                                               distribution update (default 30)
 * -nb_episodes n                                number of episodes (default 200)
 * -rho x                                        proportion of the best sample vectors to keep (default 0.1)
 * -noise {none | constant a |                   noise type and parameters (default none)
 *         linear a b | hyperbolic a}
 */
int main(int argc, char **argv) {
  CrossEntropyParameters parameters;

  if (argc == 1) {
    ask_parameters(&parameters);
  }
  else {
    parse_parameters(argc - 1, argv + 1, &parameters);
  }

  initialize_cross_entropy(&parameters);
  cross_entropy(&parameters);
  exit_cross_entropy(&parameters);

  return 0;
}
