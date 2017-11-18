/**
 * This module contains functions to parse the parameters from the command line
 * or to ask them to the user.
 */

#include "config.h"
#include "lambda_policy_iteration_parameters.h"
#include "file_tools.h"

/**
 * Prints a help message explaining how to use the program.
 */
static void print_usage(void);

/**
 * Asks all parameters to the user.
 */
void ask_parameters(LPIParameters *parameters) {
  char *read;
  char line[MAX_FILE_NAME];

  /* first ask the common parameters */
  ask_common_parameters(&parameters->common_parameters);

  
  /* gamma */
  printf("Gamma discount factor (default 1.0): ");
  read = fgets(line, MAX_LENGTH, stdin);
  if (read == NULL || line[0] == '\n') {
    parameters->gamma = 1.0;
  }
  else {
    sscanf(line, "%lf", &parameters->gamma);
  }

  /* lambda */
  printf("Lambda (default 0.6): ");
  read = fgets(line, MAX_LENGTH, stdin);
  if (read == NULL || line[0] == '\n') {
    parameters->lambda = 0.6;
  }
  else {
    sscanf(line, "%lf", &parameters->lambda);
  }

  /* number of games in a batch */
  printf("Number of games in a batch (default 100): ");
  read = fgets(line, MAX_LENGTH, stdin);
  if (read == NULL || line[0] == '\n') {
    parameters->nb_games_batch = 100;
  }
  else {
    sscanf(line, "%d", &parameters->nb_games_batch);
  }

  /* number of iterations before stop */
  printf("Number of batchs to do (default 30): ");
  read = fgets(line, MAX_LENGTH, stdin);
  if (read == NULL || line[0] == '\n') {
    parameters->nb_iterations = 30;
  }
  else {
    sscanf(line, "%d", &parameters->nb_iterations);
  }

  /* variable stepsize? */
  printf("Use variable stepsize? (y/n, default n): ");
  read = fgets(line, MAX_LENGTH, stdin);
  if (read == NULL || line[0] != 'y') {
    parameters->variable_stepsize = 0;
  }
  else {
    parameters->variable_stepsize = 1;
  }

  if (parameters->variable_stepsize) {
    /* a in the stepsize parameter  */
    printf("Stepsize parameter: gamma = a / (b + t):\n  a (default 10.0): ");
    read = fgets(line, MAX_LENGTH, stdin);
    if (read == NULL || line[0] == '\n') {
      parameters->stepsize_a = 10;
    }
    else {
      sscanf(line, "%lf", &parameters->stepsize_a);
    }
    
    /* b in stepsize parameter  */
    printf("  b (default 10.0): ");
    read = fgets(line, MAX_LENGTH, stdin);
    if (read == NULL || line[0] == '\n') {
      parameters->stepsize_b = 10;
    }
    else {
      sscanf(line, "%lf", &parameters->stepsize_b);
    }
  }

  /* initial feature file */
  printf("File to read the features with the initial weights (default bertsekas_initial.dat): ");
  read = fgets(line, MAX_LENGTH, stdin);
  if (read == NULL || line[0] == '\n') {
    strcpy(parameters->initial_feature_file_name, "bertsekas_initial.dat");
  }
  else {
    line[strlen(line)-1] = '\0'; 
    strcpy(parameters->initial_feature_file_name, line);
  }

  /* final feature file */
  printf("File to save the features with the final weights (default bertsekas.dat): ");
  read = fgets(line, MAX_LENGTH, stdin);
  if (read == NULL || line[0] == '\n') {
    strcpy(parameters->final_feature_file_name, "bertsekas.dat");
  }
  else {
    line[strlen(line)-1] = '\0'; 
    strcpy(parameters->final_feature_file_name, line);
  }

  /* statistics file */
  printf("File to save the game statistics (default lambda_statistics.dat): ");
  read = fgets(line, MAX_LENGTH, stdin);
  if (read == NULL || line[0] == '\n') {
    strcpy(parameters->statistics_file_name, "lambda_statistics.dat");
  }
  else {
    line[strlen(line)-1] = '\0'; 
    strcpy(parameters->statistics_file_name, line);
  }

  /* evaluate gameover with features */
  printf("Evaluate game-over with features (else return 0) ? (y/n, default n): ");
  read = fgets(line, MAX_LENGTH, stdin);
  if (read == NULL || line[0] != 'y') {
    parameters->gameover_evaluation = 0;
  }
  else {
    parameters->gameover_evaluation = 1;
  }

  /* bias on the end of game */
  printf("Bias the samples ? (default 0)): \n"
	 "  0. No weight\n"
	 "  1. The weight is (1-lambda)/(1-lambda^(Nm-k)\n" 
	 "  2. The weight is (1-1/game_length)^(Nm-k) 0.\n"
	 "Your choice: ");
  read = fgets(line, MAX_LENGTH, stdin);
  if (read == NULL || line[0] < '0' || line[0]>'2') {
    parameters->bias_end_of_game = 0;
  }
  else {
    parameters->bias_end_of_game = line[0] - '0';
  }

}

/**
 * Parses the parameters from the command line.
 */
void parse_parameters(int nb_args, char **args, LPIParameters *parameters) {
  int i, nb_read, recognized;

  /* first, load the default parameters */
  load_default_parameters(&parameters->common_parameters);
  parameters->nb_games_batch = 100;
  parameters->nb_iterations = 30;
  parameters->lambda = 0.6;
  parameters->gamma = 1.0;
  parameters->variable_stepsize = 0;
  parameters->stepsize_a = 10.0;
  parameters->stepsize_b = 10.0;
  strcpy(parameters->initial_feature_file_name, "bertsekas_initial.dat");
  strcpy(parameters->final_feature_file_name, "bertsekas.dat");
  strcpy(parameters->statistics_file_name, "lambda_statistics.dat");
  parameters->gameover_evaluation = 0;
  parameters->bias_end_of_game = 0;

  /* then parse the specified paramaters */
  i = 0;
  while (i < nb_args) {
    recognized = 0;

    /* number of games in an iteration */
    if (!strcmp(args[i], "-nb_games_batch")) {
      i++;
      parameters_assert(i < nb_args, "Missing argument to parameter -nb_games_batch", print_usage);
      parameters_assert(sscanf(args[i], "%d", &parameters->nb_games_batch) == 1,
			"Incorrect argument for parameter -nb_games_batch", print_usage);
      recognized = 1;
    }

    /* number of iterations */
    if (!strcmp(args[i], "-nb_iterations")) {
      i++;
      parameters_assert(i < nb_args, "Missing argument to parameter -nb_iterations", print_usage);
      parameters_assert(sscanf(args[i], "%d", &parameters->nb_iterations) == 1,
			"Incorrect argument for parameter -nb_iterations", print_usage);
      recognized = 1;
    }

    /* lambda */
    if (!strcmp(args[i], "-lambda")) {
      i++;
      parameters_assert(i < nb_args, "Missing argument to parameter -lambda", print_usage);
      parameters_assert(sscanf(args[i], "%lf", &parameters->lambda) == 1,
			"Incorrect argument for parameter -lambda", print_usage);
      recognized = 1;
    }
    
    /* gamma */
    if (!strcmp(args[i], "-gamma")) {
      i++;
      parameters_assert(i < nb_args, "Missing argument to parameter -gamma", print_usage);
      parameters_assert(sscanf(args[i], "%lf", &parameters->gamma) == 1,
			"Incorrect argument for parameter -gamma", print_usage);
      recognized = 1;
    }


    /* stepsize */
    if (!strcmp(args[i], "-stepsize")) {
      i++;
      parameters_assert(i < nb_args, "Missing arguments to parameter -stepsize", print_usage);
      parameters_assert(sscanf(args[i], "%lf", &parameters->stepsize_a) == 1,
			"Incorrect argument a for parameter -stepsize", print_usage);

      i++;
      parameters_assert(i < nb_args, "Missing the second argument to parameter -stepsize", print_usage);
      parameters_assert(sscanf(args[i], "%lf", &parameters->stepsize_b) == 1,
			"Incorrect argument b for parameter -stepsize", print_usage);

      parameters->variable_stepsize = 1;
      recognized = 1;
    }

    /* initial feature file */
    if (!strcmp(args[i], "-initial_features")) {
      i++;
      parameters_assert(i < nb_args, "Missing argument to parameter -initial_features", print_usage);
      parameters_assert(sscanf(args[i], "%s", parameters->initial_feature_file_name) == 1,
			"Incorrect argument for parameter -initial_features", print_usage);
      recognized = 1;
    }

    /* final feature file */
    if (!strcmp(args[i], "-final_features")) {
      i++;
      parameters_assert(i < nb_args, "Missing argument to parameter -final_features", print_usage);
      parameters_assert(sscanf(args[i], "%s", parameters->final_feature_file_name) == 1,
			"Incorrect argument for parameter -final_features", print_usage);
      recognized = 1;
    }

    /* statistics file */
    if (!strcmp(args[i], "-statistics_file")) {
      i++;
      parameters_assert(i < nb_args, "Missing argument to parameter -statistics_file", print_usage);
      parameters_assert(sscanf(args[i], "%s", parameters->statistics_file_name) == 1,
			"Incorrect argument to parameter -statistics_file", print_usage);
      recognized = 1;
    }
    
    /*  gameover_evaluation */
    if (!strcmp(args[i], "-gameover_evaluation")) {
      parameters->gameover_evaluation = 1;
      recognized = 1;
    }

    /* bias_end_of_game */
    if (!strcmp(args[i], "-bias_end_of_game")) {
      i++;
      parameters_assert(i < nb_args, "Missing argument to parameter -bias_end_of_game", print_usage);
      parameters_assert(sscanf(args[i], "%d", &parameters->bias_end_of_game) == 1,
			"Incorrect argument for parameter -bias_end_of_game", print_usage);
      recognized = 1;
    }

    /* if the parameter is none of these ones, it should be a common parameter */
    if (!recognized) {
      nb_read = parse_common_parameter(&parameters->common_parameters, nb_args - i, &args[i], print_usage);
      recognized = (nb_read > 0);
      parameters_assert(recognized, "Unknown parameter", print_usage);
      
      i += nb_read;
    }

    else {
      i++;
    }
  }
}

/**
 * Prints a help message explaining how to use the program.
 */
static void print_usage(void) {

  /* the whole string is too long for ISO C89 compilers */

  fprintf(stderr,
	  "Usage: ./lambda_policy_iteration [parameters]\n"
	  "\n"
	  "Without arguments, all parameters are asked to the user.\n"
	  "If there is at least one argument, the missing parameters take their default values.\n\n");

  common_parameters_print_usage();


  fprintf(stderr,
	  "-nb_games_batch n                             number of games in an iteration (default 10)\n"
	  "-nb_iterations n                              number of iterations (default 30)\n"
	  "-lambda x                                     lambda (default 0.6)\n"
	  "-gamma x                                      gamma discount factor (default 1.0)\n"
	  "-stepsize a b                                 stepsize parameter: a / (b + t) (default stepsize=1)\n\n");

  fprintf(stderr,
	  "-initial_features file_name                   file describing the features to use and their initial weights\n"
	  "-final_features file_name                     file where the final weights will be saved\n"
	  "-statistics_file file_name                    file where the game statistics at each iteration will be saved\n\n");

  fprintf(stderr,
	  "-gameover_evaluation                          the value of a gameover state is computed from the features (default: not set, the value is 0)\n\n");
  
  fprintf(stderr,
          "-bias_end_of_game                             0:None / 1:(1-gamma.lambda)/(1-(gamma.lambda)^(Nm-k) / 2:(gamma*(1-1/game_length))^(Nm-k) (default: 0)\n");

}
