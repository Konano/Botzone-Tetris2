#ifndef CROSS_ENTROPY_PARAMETERS_H
#define CROSS_ENTROPY_PARAMETERS_H

#include "types.h"
#include "common_parameters.h"
#include "cross_entropy_noise.h"
#include "feature_policy.h"

typedef struct CrossEntropyParameters {
  CommonParameters common_parameters;

  FeaturePolicy feature_policy;                  /* the feature based policy used */

  char initial_feature_file_name[MAX_FILE_NAME]; /* name of the file where the features and
					            their initial distributions are read */
  char current_feature_file_name[MAX_FILE_NAME]; /* name of the file where the features and
					            their current weights are saved */
  char final_feature_file_name[MAX_FILE_NAME];   /* name of the file where the features and
					            their final weights (i.e. the best ones) are saved */
  char statistics_file_name[MAX_FILE_NAME];      /* name of the file where the statistics for
					            each game batch are saved */

  int nb_vectors_generated;                      /* number of sample feature vectors to generate
						    at each iteration */
  int nb_evaluating_games;                       /* number of games played to evaluate a vector generated */
  int board_height_when_evaluating;               /* height of the board for the games played to evaluate a vector */
  int nb_games_after_update;                     /* number of games to play with the mean weights after a
						    distribution update */
  int nb_episodes;                               /* number of episodes */
  double rho;                                    /* proportion of the best sample vectors to keep */

  NoiseFunction *noise_function;                 /* noise function: none, constant, decreasing or hyperbolically decreasing */
  NoiseParameters noise_parameters;              /* noise paramaters a and b */

  double *means;                                 /* parameter mu of the distribution */
  double *variances;                             /* parameter sigma2 of the distribution */
  
} CrossEntropyParameters;

/**
 * Asks all parameters to the user.
 */
void ask_parameters(CrossEntropyParameters *parameters);

/**
 * Parses the parameters from the command line.
 */
void parse_parameters(int nb_args, char **args, CrossEntropyParameters *parameters);

/**
 * Loads the features and the initial distribution of their weights from the file specified
 * in the parameters.
 */
void cross_entropy_load_feature_policy(CrossEntropyParameters *parameters);

/**
 * Saves the feature based policy and the distribution of the weights into a file.
 */
void cross_entropy_save_feature_policy(const char *feature_file_name, const  CrossEntropyParameters *parameters);


#endif
