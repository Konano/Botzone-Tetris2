#ifndef LAMBDA_POLICY_ITERATION_PARAMETERS_H
#define LAMBDA_POLICY_ITERATION_PARAMETERS_H

#include <string.h>
#include "common_parameters.h"
#include "feature_policy.h"
#include "rewards.h"
#include "macros.h"

/**
 * Algorithm parameters.
 */
typedef struct LPIParameters {
  CommonParameters common_parameters;            /* board dimensions, pieces, reward function */

  FeaturePolicy feature_policy;                      /* the features used */

  int nb_games_batch;                            /* number of games in a batch */
  int nb_iterations;                             /* maximum number of iterations */
  double lambda;                                 /* parameter lambda */

  int variable_stepsize;                         /* use a variable stepsize parameter: */
  double stepsize_a;                             /* a in the stepsize parameter: gamma = a / (b + t) */
  double stepsize_b;                             /* b in the stepsize parameter */

  char initial_feature_file_name[MAX_FILE_NAME]; /* name of the file where the features and
					            their initial weights are read */
  char final_feature_file_name[MAX_FILE_NAME];   /* name of the file where the features and
					            their final weights are saved */
  char statistics_file_name[MAX_FILE_NAME];      /* name of the file where the statistics for
					            each game batch are saved */

  int gameover_evaluation;                       /* 1 if the value of a gameover state is computed with the features
						  * (else it is 0) */

  int bias_end_of_game;                          /* Allows to put more weights on the last samples of a game:
						    0: No weighing
						    1: the weight is (1-lambda)/(1-lambda^(Nm-k) 
						    2: the weight is (1-1/game_length)^(Nm-k) 
						    */
  double gamma;                                  /* Discount factor */


} LPIParameters;

/**
 * Asks all parameters to the user.
 */
void ask_parameters(LPIParameters *parameters);

/**
 * Parses the parameters from the command line.
 */
void parse_parameters(int nb_args, char **args, LPIParameters *parameters);

/**
 * Prints a help message explaining how to use the program and exits the program.
 */
void print_usage_and_quit(void);

#endif
