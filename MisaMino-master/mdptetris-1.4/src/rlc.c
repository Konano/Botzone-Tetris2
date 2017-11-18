#include <math.h>
#include <gsl/gsl_sort_double.h>
#include "config.h"
#include "rlc_parameters.h"
#include "feature_functions.h"
#include "estimate_duration.h"
#include "interruptions.h"
#include "random.h"

/*static char *feature_strings[] = {"dtp0","dtp1","dtpm","dtpn1","dtpn","Ddtp0","Ddtp1","Ddtpm","Ddtpn1","Ddtpn","landing_height","eroded_piece_cells","row_transitions","column_transitions","hole_number","well_sums","dtp","hole_depths","rows_with_holes","dtp * dtp","hole_number * hole_number","???","???","???","???","???"};*/

/*static char *feature_strings[] = {"landing_height","eroded_piece_cells","row_transitions","column_transitions","hole_number","well_sums","hole_depths","rows_with_holes","???"};*/

/*static char *feature_strings[] = {"height0","height1","heightm","heightn1","heightn","Ddtp0","Ddtp1","Ddtpm","Ddtpn1","Ddtpn","landing_height","eroded_piece_cells","row_transitions","column_transitions","hole_number","well_sums","max_height","hole_depths","rows_with_holes","dtp * dtp","hole_number * hole_number","???","???","???","???","???"}; */

static void initialize_rlc(RLCParameters *parameters);
static void exit_rlc(RLCParameters *parameters);
static void rlc(RLCParameters *parameters);
static void save_current_features(RLCParameters *parameters);

static void generate_feature_sample(const RLCParameters *parameters, FeaturePolicy *feature_policies);
static double evaluate_feature_sample(const RLCParameters *parameters, const FeaturePolicy *feature_policy);

static void update_distribution(RLCParameters *parameters, FeaturePolicy *feature_policies,
				double *evaluations, int nb_vectors_kept, size_t *best_vector_indices,
				int episode_number);
static double rlc_evaluate(const RLCParameters *parameters,const FeaturePolicy *feature_policy, int nb_moves);


/**
 * Initializes the algorithm.
 * This function has to be called before rlc() is called.
 * The parameters should have been set with ask_parameters() or parse_parameters().
 */
static void initialize_rlc(RLCParameters *parameters) {
  /* initialize the random number generator */
  initialize_random_generator(parameters->common_parameters.random_generator_seed);
  
  /* initialize the features */
  rlc_load_feature_policy(parameters);
  features_initialize(&parameters->feature_policy);
}

/**
 * Frees the memory.
 */
static void exit_rlc(RLCParameters *parameters) {
  exit_random_generator();
  FREE(parameters->means);
  FREE(parameters->variances);
  FREE(parameters->feature_policy.features);
}

/**
 * Executes the algorithm.
 * initialize_rlc() should have been called first.
 */
static void rlc(RLCParameters *parameters) {
  int i,  nb_features, nb_episodes, nb_vectors_kept;
  FeaturePolicy *feature_policies;
  double *evaluations, mean_performance, performance, best_performance;
  size_t *best_vector_indices;
  double *best_feature_values;

  /* allocate memory */
  nb_features = parameters->feature_policy.nb_features;
  nb_vectors_kept = (int) (parameters->nb_vectors_generated * parameters->rho);

  CALLOC(feature_policies, FeaturePolicy, parameters->nb_vectors_generated);
  MALLOCN(evaluations, double, parameters->nb_vectors_generated);
  MALLOCN(best_vector_indices, size_t, nb_vectors_kept);

  MALLOCN(best_feature_values, double, parameters->feature_policy.nb_features);

  /* initialize Ctrl-C handling */
  initialize_interruptions();

  nb_episodes=0;

  best_performance = 0;

  do {
    /* generate the vectors of features */
    printf("Generating %d feature vectors\n", parameters->nb_vectors_generated);
    for (i = 0; i < parameters->nb_vectors_generated; i++) {
      generate_feature_sample(parameters, &feature_policies[i]);
    }
    
    /* evaluate each vector */

    /* printf("Evaluating each vector on a %d*%d board\n",  parameters->common_parameters.board_width, parameters->board_height_when_evaluating);*/

    /* Fahey's method
    printf("Evaluating each vector, playing %d moves on a %d*%d board\n", 100000, parameters->common_parameters.board_width, parameters->board_height_when_evaluating);
    */

    mean_performance = 0;
    for (i = 0; i < parameters->nb_vectors_generated; i++) {
      printf("*%i/%i* ",i,parameters->nb_vectors_generated-1); 
      evaluations[i] = evaluate_feature_sample(parameters, &feature_policies[i]);
      mean_performance += evaluations[i];
      /*      printf("\n");*/
    }
    printf("\n");
    mean_performance /= parameters->nb_vectors_generated;
    printf("Mean performance of the generated vectors: %f\n", mean_performance);
    
    /* keep the rho * n best vectors */
    gsl_sort_largest_index(best_vector_indices, nb_vectors_kept, evaluations, 1, parameters->nb_vectors_generated);

    printf("keeping %d vectors:", nb_vectors_kept);
    for (i = 0; i < nb_vectors_kept; i++) {
      printf(" %f",  evaluations[best_vector_indices[i]]);
    }
    printf("\n");
    fflush(stdout);

    /*    printf("Features of the best vector:\n");
    print_features(&feature_policies[best_vector_indices[0]]);
    */

    /* update mu and sigma2 */
    /*printf("Updating the distribution\n");*/
    update_distribution(parameters, feature_policies, evaluations, nb_vectors_kept, best_vector_indices, nb_episodes);

    /* After the first episode: assign the reward and the 
       gameover_evaluation parameters to the first computed policy: this can be useful 
       when the parameters given to the algorithm are different from those of the initial policy */
    if (nb_episodes == 0) {
      parameters->feature_policy.reward_description = parameters->common_parameters.reward_description;
      parameters->feature_policy.gameover_evaluation = 1;
    }

    /* old way: play nb_games_after_update games */
    /* printf("Playing %d games with the mean feature weights on a %d*%d board \n",
	   parameters->nb_games_after_update, parameters->common_parameters.board_width,
	   parameters->common_parameters.board_height); 

    performance = feature_policy_play_games(&parameters->feature_policy, parameters->nb_games_after_update,
					    game, games_statistics, 1);
 
    printf("\nMean score: %f\n", performance);    */
    
    /* other way: estimate the performance with Fahey's method
    printf("Estimating the performance of the mean weight policy\n");
    performance = play_moves_estimate_duration(game, &parameters->feature_policy, 1000000, NULL);
    games_statistics_add_game(games_statistics, performance);
    games_statistics_end_episode(games_statistics, &parameters->feature_policy);
    printf("Estimation = %f\n", performance);
    */

    
    /****************/

    printf("Evaluation_of_the_mean_vector:\n");
    performance=rlc_evaluate(parameters,&(parameters->feature_policy),parameters->time_after_update);
    printf("\n%i %f ###\n",nb_episodes,performance);
    fflush(stdout);

    /********************************/


    /* save the feature policy if it is the best one ever seen */
    if (performance > best_performance) {
      best_performance = performance;
      save_feature_policy(parameters->final_feature_file_name, &parameters->feature_policy);
      for (i=0; i<parameters->feature_policy.nb_features; i++) {
	best_feature_values[i]=parameters->feature_policy.features[i].weight;
      }
    }

    /* save the current feature distributions anyway in the current feature file
     * (useful if the execution is stopped and resumed later)
     */
    save_current_features(parameters);

    nb_episodes++;

  } while (!is_interrupted() && nb_episodes < parameters->nb_episodes);
  
  /*  printf("\neval = lines_removed \\ \n");
  for (i=0; i<parameters->feature_policy.nb_features; i++) {    
    printf("       %+e * %s \\ \n",best_feature_values[i],feature_strings[i]);
  }
  fflush(stdout);*/

  /* free everything */
  /*games_statistics_free(games_statistics);
  free_game(game);
  FREE(evaluations);
  FREE(best_vector_indices);

  for (i = 0; i < parameters->nb_vectors_generated; i++) {
    FREE(feature_policies[i].features);
  }
  FREE(feature_policies);*/

  

}

/**
 * Saves in a file the current mean and variance of each feature.
 * The file name is given by parameters->current_features_file_name.
 * This is useful to resume the execution later if it is stopped.
 * To resume the execution later, rerun the program with this file as initial feature file.
 */
static void save_current_features(RLCParameters *parameters) {
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
static void generate_feature_sample(const RLCParameters *parameters, FeaturePolicy *feature_policy) {
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


static double rlc_evaluate(const RLCParameters *parameters,const FeaturePolicy *feature_policy, int nb_moves) {

  Game *game;
  Action action;
  int evaluation;
  int i,j,k,sum,lost_games,nb_removed_lines;
  int w=parameters->common_parameters.board_width,h=parameters->common_parameters.board_height;   

  evaluation=0;
        	
  game= new_game(1, w,h,parameters->common_parameters.allow_lines_after_overflow,parameters->common_parameters.piece_file_name,NULL);  
  
  lost_games=0;
  
  i=0;
  while (i<nb_moves) {    
    
    /* Biasing the current piece with the distribution dist */
    j=random_uniform(0, parameters->sum_dist);
    k=0;
    sum=parameters->dist[0];
    while (sum<=j) {
      k++;
      sum += parameters->dist[k];      
    }
    /*printf("%i\n",k);*/
    game_set_current_piece_index(game, k);

    /* find the best action */
    features_get_best_action(game, feature_policy, &action);

    /* doing the move */
    nb_removed_lines = game_drop_piece(game, &action, 0);    

    /* incrementation of the number of steps in the RLC fashion */
    i += game->last_move_info.nb_steps;

    evaluation += parameters->scores[nb_removed_lines];
    if (game->game_over) {      
      lost_games++;
      game_reset(game);
      i += 2; /* lost game = one iteration */
    }    
  }
  printf("%ix%i (%i,%i)  ", w, h, evaluation,lost_games);
  
  free_game(game);
  
  fflush(stdout);
  
  return (double)evaluation;
}

/**
 * Plays games with a specific feature vector to evaluate it.
 */
static double evaluate_feature_sample(const RLCParameters *parameters, const FeaturePolicy *feature_policy) {

  return rlc_evaluate(parameters,feature_policy,parameters->time);
}

/**
 * Updates mu and sigma2 according to the best rho * n feature vectors generated.
 */
static void update_distribution(RLCParameters *parameters, FeaturePolicy *feature_policies,
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
  printf("\nEpisode %i (noise=%f)\nNew mu and sigma2:\n",episode_number,parameters->noise_function(&parameters->noise_parameters, episode_number));
  for (i = 0; i < nb_features; i++) {
    printf("%d: mu = %f, sigma2 = %f\n", parameters->feature_policy.features[i].feature_id,
	   parameters->means[i], parameters->variances[i]);
  }
}

/**
 * Main function.
 * Usage: ./rlc [parameters]
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
  RLCParameters parameters;

  if (argc == 1) {
    ask_parameters(&parameters);
  }
  else {
    parse_parameters(argc - 1, argv + 1, &parameters);
  }

  initialize_rlc(&parameters);
  rlc(&parameters);
  exit_rlc(&parameters);

  return 0;
}
