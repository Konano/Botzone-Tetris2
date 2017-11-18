#include <time.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_sort_double.h>
#include "config.h"
#include "random.h"

static gsl_rng *gsl_random_generator = NULL;

/**
 * Initializes the GSL random number generator with a specified seed.
 */
void initialize_random_generator(unsigned int seed) {
  printf("# Initializing the random number generator with seed %d\n", seed);

  if (gsl_random_generator == NULL) {
    gsl_random_generator = gsl_rng_alloc(gsl_rng_taus);
  }

  gsl_rng_set(gsl_random_generator, seed);
}

/**
 * Frees the random number generator.
 */
void exit_random_generator() {
  gsl_rng_free(gsl_random_generator);
}

/**
 * Returns an integer number in [a,b[.
 */
int random_uniform(int a, int b) {
  return gsl_ran_flat(gsl_random_generator, a, b);
}

/**
 * Returns a gaussian random number wwith mean mu and standard deviation sigma.
 */
double random_gaussian(double mu, double sigma) {
  return gsl_ran_gaussian(gsl_random_generator, sigma) + mu;
}
