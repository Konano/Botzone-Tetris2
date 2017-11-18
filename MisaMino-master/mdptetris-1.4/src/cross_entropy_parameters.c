#include <stdio.h>
#include <string.h>
#include "config.h"
#include "cross_entropy_parameters.h"
#include "feature_functions.h"
#include "rewards.h"
#include "file_tools.h"

/**
 * Prints a message explaining the command line syntax.
 */
static void print_usage(void);

/**
 * Asks all parameters to the user.
 */
void ask_parameters(CrossEntropyParameters *parameters) {
  char *read;
  char line[MAX_LENGTH];
  NoiseFunctionID noise_function_id;

  /* first ask the common parameters */
  set_default_reward_function(NO_REWARD);
  ask_common_parameters(&parameters->common_parameters);

  /* initial feature file */
  printf("File to read the features with the initial distribution of their weights (default features/ce_bertsekas.dat): ");
  read = fgets(line, MAX_LENGTH, stdin);
  if (read == NULL || line[0] == '\n') {
    strcpy(parameters->initial_feature_file_name, "features/ce_bertsekas.dat");
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
  
  /* current feature file (the name is determined from the final feature file) */
  sprintf(parameters->current_feature_file_name, "%s.current", parameters->final_feature_file_name);

  /* statistics file */
  printf("File to save the game statistics (default ce_statistics.dat): ");
  read = fgets(line, MAX_LENGTH, stdin);
  if (read == NULL || line[0] == '\n') {
    strcpy(parameters->statistics_file_name, "ce_statistics.dat");
  }
  else {
    line[strlen(line)-1] = '\0'; 
    strcpy(parameters->statistics_file_name, line);
  }

  /* number of sample vectors */
  printf("Number of sample vectors to generate at each iteration (default 100): ");
  read = fgets(line, MAX_LENGTH, stdin);
  if (read == NULL || line[0] == '\n') {
    parameters->nb_vectors_generated = 100;
  }
  else {
    sscanf(line, "%d", &parameters->nb_vectors_generated);
  }


  /* number of games played to evaluate a generated vector */
  printf("Number of games played to evaluate each generated vector (default 1): ");
  read = fgets(line, MAX_LENGTH, stdin);
  if (read == NULL || line[0] == '\n') {
    parameters->nb_evaluating_games = 1;
  }
  else {
    sscanf(line, "%d", &parameters->nb_evaluating_games);
  }

  /* height of the board when playing games to evaluate a generated vector */
  printf("Height of the board when playing games to evaluate a generated vector (default 10): ");
  read = fgets(line, MAX_LENGTH, stdin);
  if (read == NULL || line[0] == '\n') {
    parameters->board_height_when_evaluating = 10;
  }
  else {
    sscanf(line, "%d", &parameters->board_height_when_evaluating);
  }

  /* number of games to play after an update  */
  printf("Number of games to play after a distribution update (default 30): ");
  read = fgets(line, MAX_LENGTH, stdin);
  if (read == NULL || line[0] == '\n') {
    parameters->nb_games_after_update = 30;
  }
  else {
    sscanf(line, "%d", &parameters->nb_games_after_update);
  }

  /* number of episodes */
  printf("Number of episodes (default 200): ");
  read = fgets(line, MAX_LENGTH, stdin);
  if (read == NULL || line[0] == '\n') {
    parameters->nb_episodes = 200;
  }
  else {
    sscanf(line, "%d", &parameters->nb_episodes);
  }

  /* rho */
  printf("Rho (proportion of best sample vectors to keep, default 0.1): ");
  read = fgets(line, MAX_LENGTH, stdin);
  if (read == NULL || line[0] == '\n') {
    parameters->rho = 0.1;
  }
  else {
    sscanf(line, "%lf", &parameters->rho);
  }

  /* noise */
  printf("Noise function (default 0):\n"
	 "  0. No noise\n"
	 "  1. Constant noise\n"
	 "  2. Linear decreasing noise\n"
	 "  3. Hyperbolic decreasing noise\n"
	 "Your choice: ");
  read = fgets(line, MAX_LENGTH, stdin);
  if (read == NULL || line[0] < '0' || line[0] > '3') {
    noise_function_id = NOISE_NONE;
  }
  else {
    noise_function_id = line[0] - '0';
  }
  parameters->noise_function = all_noise_functions[noise_function_id];
  
  /* noise parameters */
  switch (noise_function_id) {
    
  case NOISE_NONE:
    break;

  case NOISE_CONSTANT:
    printf("  Noise value (default 4.0): ");
    read = fgets(line, MAX_LENGTH, stdin);
    if (read == NULL || line[0] == '\n') {
      parameters->noise_parameters.a = 4.0;
    }
    else {
      sscanf(line, "%lf", &parameters->noise_parameters.a);
    }
    break;

  case NOISE_LINEAR:
    printf("  Noise = max(a - (b / t), 0):\n    a (default 5.0): ");
    read = fgets(line, MAX_LENGTH, stdin);
    if (read == NULL || line[0] == '\n') {
      parameters->noise_parameters.a = 5.0;
    }
    else {
      sscanf(line, "%lf", &parameters->noise_parameters.a);
    }
    printf("    b (default 10.0): ");
    read = fgets(line, MAX_LENGTH, stdin);
    if (read == NULL || line[0] == '\n') {
      parameters->noise_parameters.b = 10.0;
    }
    else {
      sscanf(line, "%lf", &parameters->noise_parameters.b);
    }
    break;

  case NOISE_HYPERBOLIC:
    printf("  Noise = a / t:\n    a (default 20.0): ");
    read = fgets(line, MAX_LENGTH, stdin);
    if (read == NULL || line[0] == '\n') {
      parameters->noise_parameters.a = 20.0;
    }
    else {
      sscanf(line, "%lf", &parameters->noise_parameters.a);
    }
    break;

  }
  
}

/**
 * Parses the parameters from the command line.
 */
void parse_parameters(int nb_args, char **args, CrossEntropyParameters *parameters) {
  char noise_function_name[MAX_LENGTH];
  NoiseFunctionID noise_function_id=NOISE_NONE;
  int i, nb_read, recognized;

  /* load the default parameters... */
  set_default_reward_function(NO_REWARD);
  load_default_parameters(&parameters->common_parameters);

  strcpy(parameters->initial_feature_file_name, "features/ce_bertsekas.dat");
  strcpy(parameters->final_feature_file_name, "bertsekas.dat");
  strcpy(parameters->statistics_file_name, "ce_statistics.dat");
  parameters->nb_vectors_generated = 100;;
  parameters->nb_evaluating_games = 1;
  parameters->board_height_when_evaluating = 10;
  parameters->nb_games_after_update = 30;
  parameters->nb_episodes = 200;
  parameters->rho = 0.1;
  parameters->noise_function = all_noise_functions[NOISE_NONE];
  
  /* then parse the specified paramaters */
  i = 0;
  while (i < nb_args) {
    recognized = 0;

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

    /* current feature file (the name is determined from the final feature file) */
    sprintf(parameters->current_feature_file_name, "%s.current", parameters->final_feature_file_name);

    /* statistics file */
    if (!strcmp(args[i], "-statistics_file")) {
      i++;
      parameters_assert(i < nb_args, "Missing argument to parameter -statistics_file", print_usage);
      parameters_assert(sscanf(args[i], "%s", parameters->statistics_file_name) == 1,
			"Incorrect argument to parameter -statistics_file", print_usage);
      recognized = 1;
    }
    
    /* number of sample vectors to generate at each iteration */
    if (!strcmp(args[i], "-nb_vectors_generated")) {
      i++;
      parameters_assert(i < nb_args, "Missing argument to parameter -nb_vectors_generated", print_usage);
      parameters_assert(sscanf(args[i], "%d", &parameters->nb_vectors_generated) == 1,
			"Incorrect argument for parameter -nb_vectors_generated", print_usage);
      recognized = 1;
    }

    /* number of games played to evaluate each generated vector */
    if (!strcmp(args[i], "-nb_evaluating_games")) {
      i++;
      parameters_assert(i < nb_args, "Missing argument to parameter -nb_evaluating_games", print_usage);
      parameters_assert(sscanf(args[i], "%d", &parameters->nb_evaluating_games) == 1,
			"Incorrect argument for parameter -nb_evaluating_games", print_usage);
      recognized = 1;
    }

    /* height of the board when playing games to evaluate a generated vector */
    if (!strcmp(args[i], "-board_height_when_evaluating")) {
      i++;
      parameters_assert(i < nb_args, "Missing argument to parameter -board_height_when_evaluating", print_usage);
      parameters_assert(sscanf(args[i], "%d", &parameters->board_height_when_evaluating) == 1,
			"Incorrect argument for parameter -board_height_when_evaluating", print_usage);
      recognized = 1;
    }

    /* number of games played after a distribution update */
    if (!strcmp(args[i], "-nb_games_after_update")) {
      i++;
      parameters_assert(i < nb_args, "Missing argument to parameter -nb_games_after_update", print_usage);
      parameters_assert(sscanf(args[i], "%d", &parameters->nb_games_after_update) == 1,
			"Incorrect argument for parameter -nb_games_after_update", print_usage);
      recognized = 1;
    }

    /* number of episodes */
    if (!strcmp(args[i], "-nb_episodes")) {
      i++;
      parameters_assert(i < nb_args, "Missing argument to parameter -nb_episodes", print_usage);
      parameters_assert(sscanf(args[i], "%d", &parameters->nb_episodes) == 1,
			"Incorrect argument for parameter -nb_episodes", print_usage);
      recognized = 1;
    }

    /* rho */
    if (!strcmp(args[i], "-rho")) {
      i++;
      parameters_assert(i < nb_args, "Missing argument to parameter -rho", print_usage);
      parameters_assert(sscanf(args[i], "%lf", &parameters->rho) == 1,
			"Incorrect argument for parameter -rho", print_usage);
      recognized = 1;
    }

    /* noise */
    if (!strcmp(args[i], "-noise")) {
      i++;
      parameters_assert(nb_args > 1, "Missing argument to parameter -noise", print_usage);
    
      parameters_assert(sscanf(args[i], "%s", noise_function_name) == 1,
			"Incorrect argument for parameter -noise", print_usage);
          
      if (!strcmp(noise_function_name, "none")) {
	noise_function_id = NOISE_NONE;
      }
      else if (!strcmp(noise_function_name, "constant")) {
	noise_function_id = NOISE_CONSTANT;

	/* parse one parameter */
	i++;
	parameters_assert(i < nb_args, "Missing argument to constant noise", print_usage);
	parameters_assert(sscanf(args[i], "%lf", &parameters->noise_parameters.a) == 1,
			  "Incorrect argument for constant noise", print_usage);
      }
      else if (!strcmp(noise_function_name, "linear")) {
	noise_function_id = NOISE_LINEAR;

	/* parse two parameters */
	i++;
	parameters_assert(i < nb_args, "Missing argument a to linear decreasing noise", print_usage);
	parameters_assert(sscanf(args[i], "%lf", &parameters->noise_parameters.a) == 1,
			  "Incorrect argument a for linear decreasing noise", print_usage);

	i++;
	parameters_assert(i < nb_args, "Missing argument b to linear decreasing noise", print_usage);
	parameters_assert(sscanf(args[i], "%lf", &parameters->noise_parameters.b) == 1,
			  "Incorrect argument b for linear decreasing noise", print_usage);
      }
      else if (!strcmp(noise_function_name, "hyperbolic")) {
	noise_function_id = NOISE_HYPERBOLIC;

	/* parse one parameter */
	i++;
	parameters_assert(i < nb_args, "Missing argument to hyperbolic decreasing noise", print_usage);
	parameters_assert(sscanf(args[i], "%lf", &parameters->noise_parameters.a) == 1,
			  "Incorrect argument for hyperbolic decreasing noise", print_usage);
      }
      else {
	parameters_assert(0, "Unknown noise function", print_usage);
      }
      parameters->noise_function = all_noise_functions[noise_function_id];

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
 * Loads the features and the initial distribution of their weights from the file specified
 * in the parameters.
 */
void cross_entropy_load_feature_policy(CrossEntropyParameters *parameters) {
  FILE *feature_file;
  int i, nb_features, update_column_heights_needed, reward_function_id, gameover_evaluation;
  int feature_id;
  Feature *features;

  feature_file = open_data_file(parameters->initial_feature_file_name, "r");

  if (feature_file == NULL) {
    DIE1("Cannot read the feature file '%s'", parameters->initial_feature_file_name);
  }

  /* read the reward function id */
  FSCANF(feature_file, "%d", &reward_function_id);

  /* read the evaluate gameover evaluation value */
  FSCANF(feature_file, "%d", &gameover_evaluation);

  /* read the number of features */
  FSCANF(feature_file, "%d", &nb_features);
  MALLOCN(features, Feature, nb_features);
  
  MALLOCN(parameters->means, double, nb_features);
  MALLOCN(parameters->variances, double, nb_features);

  /* read each feature and its initial distribution N(mu, sigma2) */
  update_column_heights_needed = 0;
  for (i = 0; i < nb_features; i++) {
    FSCANF3(feature_file, "%d %lf %lf", &feature_id, &parameters->means[i], &parameters->variances[i]);

    features[i].feature_id = feature_id;
    features[i].get_feature_rating = feature_function(feature_id);

    if (feature_id == NEXT_COLUMN_HEIGHT || feature_id == NEXT_COLUMN_HEIGHT_DIFFERENCE) {
      update_column_heights_needed = 1;
    }
  }
  
  fclose(feature_file);

  parameters->feature_policy.gameover_evaluation = gameover_evaluation;
  parameters->feature_policy.reward_description.reward_function_id = reward_function_id;
  parameters->feature_policy.reward_description.reward_function = all_reward_functions[reward_function_id];
  parameters->feature_policy.features = features;
  parameters->feature_policy.nb_features = nb_features;
  parameters->feature_policy.update_column_heights_needed = update_column_heights_needed;
}

/**
 * Saves the feature based policy and the distribution of the weights into a file.
 */
void cross_entropy_save_feature_policy(const char *feature_file_name, const  CrossEntropyParameters *parameters) {
  FILE *feature_file;
  int i;

  feature_file = fopen(feature_file_name, "w");

  if (feature_file == NULL) {
    DIE("Unable to write the feature file");
  }

  /* write the reward function id */
  fprintf(feature_file, "%d\n", parameters->feature_policy.reward_description.reward_function_id);

  /* write the gameover evaluation value */
  fprintf(feature_file, "%d\n", parameters->feature_policy.gameover_evaluation);

  /* write the number of features */
  fprintf(feature_file, "%d\n", parameters->feature_policy.nb_features);

  /* write each feature, the mean and the variance of its weight */
  for (i = 0; i < parameters->feature_policy.nb_features; i++) {
    fprintf(feature_file, "%d %e %e\n", parameters->feature_policy.features[i].feature_id, parameters->means[i], parameters->variances[i]);
  }

  fclose(feature_file);
}



/**
 * Prints a message explaining the command line syntax.
 */
static void print_usage(void) {
  
  fprintf(stderr,
	  "Usage: ./cross_entropy [parameters]\n"
	  "\n"
	  "Without arguments, all parameters are asked to the user.\n"
	  "If there is at least one argument, the missing parameters take their default values.\n"
          "\n");

  common_parameters_print_usage();

  fprintf(stderr,
	  "-initial_features file_name                   file describing the features and the initial distribution\n"
	  "                                              of their weights (default features/ce_bertsekas.dat) \n"
	  "-final_features file_name                     file where the final weights will be saved (default\n"
	  "                                              features_bertsekas.dat)\n"
	  "-statistics_file file_name                    file where the game statistics at each iteration will be saved\n"
	  );

  fprintf(stderr,
	  "                                              (default ce_statistics.dat)\n"
	  "-nb_vectors_generated n                       number of sample feature vectors to generate at each iteration\n"
	  "                                              (default 100)\n"
	  "-nb_evaluating_games n                        number of games played to evaluate a vector generated (default 1)\n"
	  );

  fprintf(stderr,
	  "-board_height_when_evaluating n               height of the board when playing games to evaluate a vector generated (default 10)\n"
	  "-nb_games_after_update n                      number of games to play with the mean weights after an update\n"
	  );

  fprintf(stderr,
	  "                                              distribution update (default 30)\n"
	  "-nb_episodes n                                number of episodes (default 200)\n"
	  "-rho x                                        proportion of the best sample vectors to keep (default 0.1)\n"
          "-noise [none | constant a |\n"
	  "        linear a b | hyperbolic a]            noise type and parameters:\n"
	  );

  fprintf(stderr,
	  "  none:           no noise\n"
	  "  constant a:     constant noise with value a\n"
	  "  linear a b:     linear decreasing noise: max(a - (t / b), 0)\n"
	  "  hyperbolic a:   hyperbolically decreasing noise: a / t\n"
	  );
    
}
