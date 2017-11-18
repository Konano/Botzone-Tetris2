#ifndef RLC_PARAMETERS_H
#define RLC_PARAMETERS_H

#include "types.h"
#include "common_parameters.h"
#include "cross_entropy_noise.h"
#include "feature_policy.h"

typedef struct RLCParameters {
  CommonParameters common_parameters;

  FeaturePolicy feature_policy;                  /* the feature based policy used */

  char initial_feature_file_name[MAX_FILE_NAME]; /* name of the file where the features and
					            their initial distributions are read */
  char current_feature_file_name[MAX_FILE_NAME]; /* name of the file where the features and
					            their current weights are saved */
  char final_feature_file_name[MAX_FILE_NAME];   /* name of the file where the features and
					            their final weights (i.e. the best ones) are saved */

  int nb_vectors_generated;                      /* number of sample feature vectors to generate
						    at each iteration */
  int time;                       /* number of games played to evaluate a vector generated */

  int time_after_update;                     /* number of games to play with the mean weights after a
						    distribution update */
  int nb_episodes;                               /* number of episodes */
  double rho;                                    /* proportion of the best sample vectors to keep */

  NoiseFunction *noise_function;                 /* noise function: none, constant, decreasing or hyperbolically decreasing */
  NoiseParameters noise_parameters;              /* noise paramaters a and b */

  double *means;                                 /* parameter mu of the distribution */
  double *variances;                             /* parameter sigma2 of the distribution */
  
  int scores[5];                                /* score gained for each number of removed lines */

  int dist[7];                          /* piece distribution */
  int sum_dist;

} RLCParameters;

/**
 * Asks all parameters to the user.
 */
void ask_parameters(RLCParameters *parameters);

/**
 * Parses the parameters from the command line.
 */
void parse_parameters(int nb_args, char **args, RLCParameters *parameters);

/**
 * Loads the features and the initial distribution of their weights from the file specified
 * in the parameters.
 */
void rlc_load_feature_policy(RLCParameters *parameters);

/**
 * Saves the feature based policy and the distribution of the weights into a file.
 */
void rlc_save_feature_policy(const char *feature_file_name, const  RLCParameters *parameters);


#endif
