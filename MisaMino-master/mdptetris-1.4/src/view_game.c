#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "config.h"
#include "game.h"
#include "file_tools.h"
#include "random.h"
#include "macros.h"

int main(int argc, char **argv) {
  FILE *game_file;
  Game *game;
  int width, height, allow_lines_after_overflow, n, piece_index;
  char *piece_file_name;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s game_file_name\n", argv[0]);
    exit(1);
  }

  initialize_random_generator(time(NULL));

  game_file = open_data_file(argv[1], "r");
  
  FREAD(&width, sizeof(int), 1, game_file);
  FREAD(&height, sizeof(int), 1, game_file);
  FREAD(&allow_lines_after_overflow, sizeof(int), 1, game_file);
  FREAD(&n, sizeof(int), 1, game_file);

  MALLOCN(piece_file_name, char, n); 
  FREAD(piece_file_name, sizeof(char), n, game_file);

  game = new_game(0, width, height, allow_lines_after_overflow, piece_file_name, NULL);

  do {
    FREAD(&game->score, sizeof(int), 1, game_file);
    FREAD(&piece_index, sizeof(int), 1, game_file);
    game_set_current_piece_index(game, piece_index);
    FREAD(game->board->rows, sizeof(int), height, game_file);
    
    game_print(stdout, game);
  }
  while (!feof(game_file));

  exit_random_generator();

  return 0;
}
