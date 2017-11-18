#ifndef STRATEGY_H
#define STRATEGY_H

#include <stdio.h>
#include "types.h"
#include "game.h"

/**
 * A strategy is a module which
 * implements functions to decide where to put
 * each piece.
 */
struct Strategy {
  void (*initialize) (Game *game);
  void (*exit) (void);
  void (*decide) (Game *game, Action *best_action);
  double (*info) (Game *game);
};

int strategy_watch_game(int tetris_implementation, Strategy *strategy, int width, int height,
			int allow_lines_after_overflow, const char *piece_file_name, int *piece_sequence);
int strategy_compete(int tetris_implementation, Strategy *strategy, int width, int height,
			  int allow_lines_after_overflow, const char *piece_file_name, int *piece_sequence);
int strategy_play_game_file(int tetris_implementation, const char *file_name, Strategy *strategy, int width, int height,
			    int allow_lines_after_overflow, const char *piece_file_name, int *piece_sequence);
int strategy_play_game(Strategy *strategy, Game *game);
void strategy_play_games(int tetris_implementation, Strategy *strategy, int nb_games, int width, int height,
			int allow_lines_after_overflow, const char *piece_file_name, int *piece_sequence);

#endif
