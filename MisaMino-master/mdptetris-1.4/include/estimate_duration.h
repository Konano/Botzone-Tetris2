/**
 * @defgroup estimate_score Estimation of a strategy's performance
 * @ingroup api
 * @brief Playing moves with some strategy and estimating the probability of losing
 *
 * This module provides an interface for estimating the probability of losing with
 * some strategy, playing a number of moves with this strategy.
 */

#ifndef ESTIMATE_DURATION_H
#define ESTIMATE_DURATION_H

#include <stdio.h>
#include "feature_policy.h"

double play_moves_estimate_duration(Game *game, const FeaturePolicy *FeaturePolicy, int nb_moves, FILE *out);

#endif
