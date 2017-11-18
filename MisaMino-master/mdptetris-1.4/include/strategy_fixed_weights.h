/**
 * Strategy evaluating a position with some features defined in a file.
 */

#ifndef STRATEGY_FIXED_WEIGHTS_H
#define STRATEGY_FIXED_WEIGHTS_H

#include "types.h"
#include "strategy.h"
#include "rewards.h"

Strategy *new_strategy_fixed_weights(char *feature_file);

#endif
