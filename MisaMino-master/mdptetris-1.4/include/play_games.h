/**
 * @defgroup play_games Play Games
 * @ingroup api
 * @brief Playing games with some strategy and generating statistics
 *
 * This module provides an interface for doing statistics on some strategy.
 */

#ifndef PLAY_GAMES_H
#define PLAY_GAMES_H

#include "strategy_fixed_weights.h"
#include "games_statistics.h"



void play_games(Game *game, Strategy *strategy, int nb_games, GamesStatistics *stats);


#endif
