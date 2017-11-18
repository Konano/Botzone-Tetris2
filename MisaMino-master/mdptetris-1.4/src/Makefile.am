# C++ flags
AM_CPPFLAGS = -DDATADIR_="$(datadir)/mdptetris" -Wall -Werror -ansi -pedantic -I$(top_srcdir)/include

# executables, installed in $(prefix)/bin
bin_PROGRAMS = mdptetris mdptetris_lambda_pi mdptetris_cross_entropy mdptetris_value_iteration mdptetris_play_games mdptetris_estimate_duration mdptetris_view_game mdptetris_uct mdptetris_cmaes mdptetris_rlc

# source files needed by each executable

mdptetris_SOURCES = tetris.c hashtable.c game.c games_statistics.c last_move_info.c piece.c file_tools.c board.c brick_masks.c strategy.c strategy_random.c strategy_fixed_weights.c strategy_value_iteration.c simple_tetris.c feature_policy.c feature_functions.c rewards.c interruptions.c random.c graphics.c 

mdptetris_lambda_pi_SOURCES = lambda_policy_iteration.c lambda_policy_iteration_parameters.c common_parameters.c game.c last_move_info.c piece.c file_tools.c board.c brick_masks.c feature_policy.c feature_functions.c rewards.c interruptions.c games_statistics.c strategy_fixed_weights.c random.c

mdptetris_cross_entropy_SOURCES = cross_entropy.c cross_entropy_parameters.c common_parameters.c game.c last_move_info.c piece.c file_tools.c board.c brick_masks.c feature_policy.c feature_functions.c rewards.c interruptions.c games_statistics.c strategy_fixed_weights.c cross_entropy_noise.c random.c estimate_duration.c

mdptetris_value_iteration_SOURCES = value_iteration.c simple_tetris.c game.c last_move_info.c piece.c file_tools.c board.c brick_masks.c interruptions.c random.c

mdptetris_play_games_SOURCES = play_games.c game.c games_statistics.c last_move_info.c piece.c file_tools.c board.c brick_masks.c strategy.c strategy_fixed_weights.c feature_policy.c feature_functions.c rewards.c interruptions.c random.c graphics.c

mdptetris_estimate_duration_SOURCES = estimate_duration_main.c estimate_duration.c game.c games_statistics.c last_move_info.c piece.c file_tools.c board.c brick_masks.c strategy.c strategy_fixed_weights.c feature_policy.c feature_functions.c rewards.c interruptions.c random.c graphics.c

mdptetris_view_game_SOURCES = view_game.c game.c last_move_info.c piece.c file_tools.c board.c brick_masks.c random.c

mdptetris_uct_SOURCES = uct.c hashtable.c game.c last_move_info.c piece.c board.c brick_masks.c random.c file_tools.c feature_policy.c feature_functions.c rewards.c games_statistics.c

mdptetris_cmaes_SOURCES = cmaes.c cmaes_main.c game.c last_move_info.c piece.c board.c brick_masks.c random.c file_tools.c feature_policy.c feature_functions.c rewards.c games_statistics.c

mdptetris_rlc_SOURCES = rlc.c rlc_parameters.c common_parameters.c game.c last_move_info.c piece.c file_tools.c board.c brick_masks.c feature_policy.c feature_functions.c rewards.c interruptions.c games_statistics.c strategy_fixed_weights.c cross_entropy_noise.c random.c estimate_duration.c

# additional files to provide in a tarball release
EXTRA_DIST = \
	$(top_srcdir)/include/macros.h					\
	$(top_srcdir)/include/types.h					\
	$(top_srcdir)/include/hashtable.h				\
	$(top_srcdir)/include/random.h					\
	$(top_srcdir)/include/game.h					\
	$(top_srcdir)/include/games_statistics.h			\
	$(top_srcdir)/include/board.h					\
	$(top_srcdir)/include/brick_masks.h				\
	$(top_srcdir)/include/piece.h					\
	$(top_srcdir)/include/file_tools.h				\
	$(top_srcdir)/include/last_move_info.h				\
	$(top_srcdir)/include/rewards.h					\
	$(top_srcdir)/include/feature_policy.h				\
	$(top_srcdir)/include/feature_functions.h			\
	$(top_srcdir)/include/interruptions.h				\
	$(top_srcdir)/include/strategy.h				\
	$(top_srcdir)/include/common_parameters.h			\
	$(top_srcdir)/include/play_games.h				\
	$(top_srcdir)/include/estimate_duration.h			\
	$(top_srcdir)/include/strategy_fixed_weights.h			\
	$(top_srcdir)/include/strategy_random.h				\
	$(top_srcdir)/include/strategy_value_iteration.h		\
	$(top_srcdir)/include/simple_tetris.h				\
	$(top_srcdir)/include/lambda_policy_iteration_parameters.h	\
	$(top_srcdir)/include/cross_entropy_parameters.h		\
	$(top_srcdir)/include/cross_entropy_noise.h			\
	$(top_srcdir)/include/uct.h					\
	$(top_srcdir)/include/cmaes.h					\
	$(top_srcdir)/include/cmaes_interface.h                         \
	$(top_srcdir)/include/graphics.h                         	\
	$(top_srcdir)/include/rlc_parameters.h


# tests
check_PROGRAMS = hashtable_test
TESTS = $(check_PROGRAMS)

hashtable_test_SOURCES = hashtable.c hashtable_test.c
