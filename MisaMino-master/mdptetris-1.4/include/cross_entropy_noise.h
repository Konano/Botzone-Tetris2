/**
 * This module define the noise you can use in the noisy cross-entropy method.
 */

#ifndef CROSS_ENTROPY_NOISE_H
#define CROSS_ENTROPY_NOISE_H

/**
 * Parameters of the noise.
 * Depending on the noise type, zero, one or two parameters are used.
 */
typedef struct NoiseParameters {
  double a;
  double b;
} NoiseParameters;

/**
 * Function type for getting the noise at episode t.
 */
typedef double (NoiseFunction)(const NoiseParameters *parameters, int t);

/**
 * Constants to identify the different noise function types.
 */
typedef enum NoiseFunctionID {
  NOISE_NONE,        /* no noise */
  NOISE_CONSTANT,    /* constant noise a */
  NOISE_LINEAR,      /* decreasing noise: max(a - (t / b), 0) */
  NOISE_HYPERBOLIC   /* hyperbolically decreasing noise: a / t */
} NoiseFunctionID;

/**
 * Associates a noise function to each index.
 */
extern NoiseFunction *all_noise_functions[];

/* The noise functions */

double get_no_noise(const NoiseParameters *parameters, int t);
double get_constant_noise(const NoiseParameters *parameters, int t);
double get_linear_decreasing_noise(const NoiseParameters *parameters, int t);
double get_hyperbolic_decreasing_noise(const NoiseParameters *parameters, int t);

#endif
