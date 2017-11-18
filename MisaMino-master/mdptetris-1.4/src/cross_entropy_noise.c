#include "config.h"
#include "cross_entropy_noise.h"
#include "macros.h"

NoiseFunction *all_noise_functions[] = {
  get_no_noise,
  get_constant_noise,
  get_linear_decreasing_noise,
  get_hyperbolic_decreasing_noise
};

/**
 * Returns 0.0.
 */
double get_no_noise(const NoiseParameters *parameters, int t) {
  return 0.0;
}

/**
 * Returns a constant noise.
 * Its value is defined by parameters->a.
 */
double get_constant_noise(const NoiseParameters *parameters, int t) {
  return parameters->a;
}

/**
 * Returns a decreasing noise: max(a - (t / b), 0).
 */
double get_linear_decreasing_noise(const NoiseParameters *parameters, int t) {
  return MAX(parameters->a - (t / parameters->b), 0);
}

/**
 * Returns an hyperbolically decreasing noise: parameters->a / t.
 */
double get_hyperbolic_decreasing_noise(const NoiseParameters *parameters, int t) {
  return parameters->a / (t + 1);
}
