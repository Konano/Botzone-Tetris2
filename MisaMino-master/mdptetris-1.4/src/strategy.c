#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <GL/glut.h>
#include "config.h"
#include "strategy.h"
#include "game.h"
#include "games_statistics.h"
#include "interruptions.h"
#include "macros.h"
#include "graphics.h"
#include "brick_masks.h"




#define color(x) glColor3f(x[0],x[1],x[2]);


/* some global variables TODO: find a nicer/cleaner way to do this */

int **current_fancy_board,**current_fancy_board2;
PieceOrientation *current_pieceorientation,*current_pieceorientation2;

int current_counter;
int current_step_counter;
int current_color;
Strategy *current_strategy;
Game *current_game,*current_game2;
int current_score,current_score2;
int column, orientation;
double current_value;
char text[]="Piece courante";
char *text2;
char *filename;
int waiting;
int nb_games, tots1, tots2;

static const float colors[8][3] = {
  {0,0,0},   /* no color */
  {1,0,0},   /* I:red */
  {0,0,1},   /* O:blue */
  {.7,.3,0.2},/* T:brown */
  {0,1,1},   /* Z:cyan */
  {0,1,0},   /* S:green */
  {1,1,1},   /* J:white */
  {1,0,1}    /* L:magenta */
};
  

/*************************************************************************************************
                                    Functions for watch_game 
*************************************************************************************************/				    				   

/**
 * Draw the board
 *
 */
void display_board_watch_game(void) {

  char text[]="Piece courante",text2[20];
  int tmp;
  int i,j;

  int d=MIN((window_width)/current_game->board->width,(window_height-36)/(current_game->board->height+5)); /* size of a brick in pixel */
  
  /* the background of the board is black */
  glClearColor (0,0,0,1);
  glClear(GL_COLOR_BUFFER_BIT);  

  glPushMatrix();

  /* the upper part of the window displays the coming piece in a grey background */  

  glColor3f(.3,.3,.3);
  glPolygonMode (GL_FRONT_AND_BACK,GL_LINE);
  window_draw_rectangle(0,0,current_game->board->width*d, current_game->board->height*d);
    
  /* rectangles will be filled with the current color */
  glPolygonMode (GL_FRONT_AND_BACK,GL_FILL); /* fill polygons */
  window_draw_rectangle(0,window_height-5*d,current_game->board->width*d,5*d);
  
  glutPrint(0,window_height-16,glutFonts[6],text,1,1,1,1);

  sprintf(text2,"Score = %i",current_score);
  glutPrint(0,window_height-5*d-16,glutFonts[6],text2,1,1,1,1);

  sprintf(text2,"Valeur = %.3f",current_value);
  glutPrint(0,window_height-5*d-34,glutFonts[6],text2,1,1,1,1);

  color(colors[current_color]);

  tmp=d*(current_game->board->width-current_pieceorientation->width)/2;
  for (i = current_pieceorientation->height - 1; i>=0; i--) {   
    for (j = 0; j < current_pieceorientation->width; j++) {
      if (current_pieceorientation->bricks[current_pieceorientation->height - 1 - i] & brick_masks[j]) {
	window_draw_rectangle(tmp+j*d,window_height-((i+2)*d),d-1,d-1);
      }
    }
  }

  for (i=0; i<current_game->board->height; i++) {
    for (j=0; j<current_game->board->width; j++) {
      /*printf("%i ",board[i][j]);*/
      if ((tmp=current_fancy_board[i][j])!=0) {
	if (tmp==-1) { /* current_piece */
	  glPolygonMode (GL_FRONT_AND_BACK,GL_LINE);
	  color(colors[current_color]);
	  window_draw_rectangle(1+j*d,((i)*d),d-1,d-1);	  
	  glPolygonMode (GL_FRONT_AND_BACK,GL_FILL);
	} else if (tmp==-2) { /* lines to remove */
	  glColor3f(.8,.8,.8);
	  window_draw_rectangle(1+j*d,((i)*d),d-1,d-1);
	} else {
	  color(colors[tmp]);
	  window_draw_rectangle(1+j*d,((i)*d),d-1,d-1);
	}
      }
    }
    /* printf("\n");*/
  }
       
  glutSwapBuffers();
  
}


void strategy_watch_game_one_step(int value) {

  Action action;
  int index,i,j,i2,tmp,line_to_remove;

  current_counter++;

  tmp=0;

  for (i=0; i<current_game->board->extended_height; i++) {
    line_to_remove=0;
    for (j=0; j<current_game->board->width; j++) {
      if (current_fancy_board[i][j]==-1) { /* there is a piece to put */
	tmp=1;
	current_fancy_board[i][j]=current_color;
      } else if (current_fancy_board[i][j]==-2) { /* there is a line to remove */     
	tmp=1;
	line_to_remove=1;
      }
    }
    if (line_to_remove) {
      for (i2=i; i2<current_game->board->extended_height-1; i2++) {
	for (j=0; j<current_game->board->width; j++) {
	  current_fancy_board[i2][j]=current_fancy_board[i2+1][j];
	}
      }
      i-=1;    
    }
  }

  if (tmp==0) { /* tmp is not equal to 1 if one has just animated */

    if (current_strategy->info != NULL) {
      current_value=current_strategy->info(current_game);
    }
    
    current_strategy->decide(current_game, &action);
    
    current_score += board_drop_piece_fancy(current_game->board, &current_game->current_piece->orientations[action.orientation],
					    action.orientation, action.column,
					    &current_game->last_move_info, 0,
					    current_fancy_board);
    current_game->score=current_score;
    current_game->previous_piece_index = current_game->current_piece_index;

  } else {

    if (current_game->board->wall_height > current_game->board->height) {
      current_game->game_over = 1;
    }
    else {
      generate_next_piece(current_game);
    }
    
    current_pieceorientation = &current_game->current_piece->orientations[0];
    current_color=current_game->current_piece_index + 1;
    
    game_print(stdout, current_game);  
    current_step_counter += current_game->last_move_info.nb_steps;
    printf("%d %d\n",current_counter,current_step_counter);     
    
    if (current_game->game_over) {      
      printf("Game Over!\n");
      /*      current_game->current_piece_sequence_index++;*/
      game_print(stdout, current_game);    
      getchar();
      index = current_game->current_piece_sequence_index+2;
      game_reset(current_game);    
      current_score=0;
      current_game->current_piece_sequence_index=index;
      for (i=0; i<current_game->board->extended_height; i++) {
	for (j=0; j<current_game->board->width; j++) {
	  current_fancy_board[i][j]=0;
	}
      }      
      game_print(stdout, current_game);

      current_pieceorientation = &current_game->current_piece->orientations[0];
      current_color=current_game->current_piece_index + 1;
    }

  }

  display_board_watch_game();
  
  /*  sprintf(filename,"im%05d.pbm",current_counter);
      save_window_to_file(filename, window_width, window_height);*/
  
  glutTimerFunc(500, strategy_watch_game_one_step, 0); /* call me again in .3 second */
}



/**
 * Plays an entire game with the given strategy function.
 * Each move is displayed and the user has to press 'Enter' to continue.
 * @param strategy a strategy
 * @param width board width
 * @param height board height
 * @param allow_lines_after_overflow 1 to enable the lines completion when the piece overflows
 * @param piece_file_name description of the pieces
 * @param piece_sequence a sequence of pieces (NULL to generate the pieces randomly)
 * @return the game score
 */
int strategy_watch_game(int tetris_implementation, Strategy *strategy, int width, int height,
			int allow_lines_after_overflow, const char *piece_file_name, int *piece_sequence) {
  
  int i;

  current_counter=0;
  current_step_counter=0;
  current_score=0;

  current_strategy = strategy;

  current_game = new_game(tetris_implementation, width, height, allow_lines_after_overflow, piece_file_name, piece_sequence);
  if (current_strategy->initialize != NULL) {
    current_strategy->initialize(current_game);
  }
  CALLOC(current_fancy_board, int*, current_game->board->extended_height);
  for (i=0; i<current_game->board->extended_height; i++) {
    CALLOC(current_fancy_board[i], int, width);
  }

  CALLOC(text2,char,20);
  CALLOC(filename,char,50);

  game_print(stdout, current_game);

  current_pieceorientation = &current_game->current_piece->orientations[0];
  current_color=current_game->current_piece_index + 1;
    
  /* Launch the glut loop */
   
  glut_loop( 
	    current_game->board->width*30,(current_game->board->height+5)*30+36,
	    "MdpTetris",
	    display_board_watch_game,
	    strategy_watch_game_one_step,10,0,
	    NULL,
	    NULL
	    );
  
  /************************/
  
  if (current_strategy->exit != NULL) {
    current_strategy->exit();
  }

  current_score=current_game->score;
  free_game(current_game);

  FREE(text2);
  FREE(filename);

  return current_score;
}



/*******************************************************************************************
                                Functions for compete_game 
*******************************************************************************************/

/**
 * Draw the boards
 *
 */
void display_board_compete(void) {

  char text[]="Piece courante",text2[20];
  int tmp;
  int i,j;

  int d=MIN(window_width/(2*current_game->board->width+1),(window_height-36)/(current_game->board->height+5)); /* size of a brick in pixel */
  
  int ww2=d*(current_game->board->width+1);

  /* the background of the board is black */
  glClearColor (0,0,0,1);
  glClear(GL_COLOR_BUFFER_BIT);  

  glPushMatrix();

  /* the upper part of the window displays the coming piece in a grey background */  

  glColor3f(.3,.3,.3);
  glPolygonMode (GL_FRONT_AND_BACK,GL_LINE);
  window_draw_rectangle(0,0,ww2-d, current_game->board->height*d);
  for (i=ww2; i<2*ww2; i+=d) {
    window_draw_line(i,0,i,current_game->board->height*d);
  }
  for (j=0; j<=current_game->board->height*d; j+=d) {
    window_draw_line(ww2,j,2*ww2-d,j);
  }
    
  /* rectangles will be filled with the current color */
  glPolygonMode (GL_FRONT_AND_BACK,GL_FILL); /* fill polygons */
  window_draw_rectangle(0,window_height-5*d,current_game->board->width*d,5*d);
  window_draw_rectangle(ww2,window_height-5*d,current_game->board->width*d,5*d);

  glutPrint(0,window_height-16,glutFonts[6],text,1,1,1,1);
  glutPrint(ww2,window_height-16,glutFonts[6],text,1,1,1,1);

  if (nb_games==0) {
    sprintf(text2,"Score: %i",current_score);
    glutPrint(0,window_height-5*d-16,glutFonts[6],text2,1,1,1,1);
    sprintf(text2,"Score: %i",current_score2);
    glutPrint(ww2,window_height-5*d-16,glutFonts[6],text2,1,1,1,1);
  } else {
    sprintf(text2,"Score: %i (moy=%f)",current_score,(float)tots1/nb_games);
    glutPrint(0,window_height-5*d-16,glutFonts[6],text2,1,1,1,1);
    sprintf(text2,"Score: %i (moy=%f)",current_score2,(float)tots2/nb_games);
    glutPrint(ww2,window_height-5*d-16,glutFonts[6],text2,1,1,1,1);
  }

  sprintf(text2,"Valeur = %.3f",current_value);
  glutPrint(0,window_height-5*d-34,glutFonts[6],text2,1,1,1,1);

  /* drawing the piece */
  color(colors[current_color]);

  if (current_game->game_over!=1) {
    tmp=d*(current_game->board->width-current_pieceorientation->width)/2;
    for (i = current_pieceorientation->height - 1; i>=0; i--) {   
      for (j = 0; j < current_pieceorientation->width; j++) {
	if (current_pieceorientation->bricks[current_pieceorientation->height - 1 - i] & brick_masks[j]) {
	  window_draw_rectangle(tmp+j*d,window_height-((i+2)*d),d-1,d-1);
	}
      }
    }
  }
  if (current_game2->game_over!=1) {
    tmp=ww2+d*column;
    for (i = current_pieceorientation2->height - 1; i>=0; i--) {   
      for (j = 0; j < current_pieceorientation2->width; j++) {
	if (current_pieceorientation2->bricks[current_pieceorientation2->height - 1 - i] & brick_masks[j]) {
	  window_draw_rectangle(tmp+j*d,window_height-((i+2)*d),d-1,d-1);
	}
      }
    }
  }
  
  for (i=0; i<current_game->board->height; i++) {
    for (j=0; j<current_game->board->width; j++) {
      if ((tmp=current_fancy_board[i][j])!=0) {
	if (tmp==-1) { /* current_piece */
	  glPolygonMode (GL_FRONT_AND_BACK,GL_LINE);
	  color(colors[current_color]);
	  window_draw_rectangle(1+j*d,((i)*d),d-1,d-1);	  
	  glPolygonMode (GL_FRONT_AND_BACK,GL_FILL);
	} else if (tmp==-2) { /* lines to remove */
	  glColor3f(.8,.8,.8);
	  window_draw_rectangle(1+j*d,((i)*d),d-1,d-1);
	} else {
	  color(colors[tmp]);
	  window_draw_rectangle(1+j*d,((i)*d),d-1,d-1);
	}
      }
    }   
  }
  for (i=0; i<current_game2->board->height; i++) {
    for (j=0; j<current_game2->board->width; j++) {
      if ((tmp=current_fancy_board2[i][j])!=0) {
	if (tmp==-1) { /* current_piece */
	  glPolygonMode (GL_FRONT_AND_BACK,GL_LINE);
	  color(colors[current_color]);
	  window_draw_rectangle(ww2+1+j*d,((i)*d),d-1,d-1);	  
	  glPolygonMode (GL_FRONT_AND_BACK,GL_FILL);
	} else if (tmp==-2) { /* lines to remove */
	  glColor3f(.8,.8,.8);
	  window_draw_rectangle(ww2+1+j*d,((i)*d),d-1,d-1);
	} else {
	  color(colors[tmp]);
	  window_draw_rectangle(ww2+1+j*d,((i)*d),d-1,d-1);
	}
      }
    }
   
  }
       
  glutSwapBuffers();
  
}

void key_pressed_compete (unsigned char key, int x, int y);


void strategy_compete_only_computer(int value) {

  key_pressed_compete(' ',0,0); /* hit next piece! */

}

void strategy_compete_next(int value) {

  int i,i2,j,line_to_remove;

  /* update fancy board 1 */
  for (i=0; i<current_game->board->extended_height; i++) {
    line_to_remove=0;
    for (j=0; j<current_game->board->width; j++) {
      if (current_fancy_board[i][j]==-1) { /* there is a piece to put */
	current_fancy_board[i][j]=current_color;	
      } else if (current_fancy_board[i][j]==-2) { /* there is a line to remove */     
	line_to_remove=1;
      }      
    }
    if (line_to_remove) {
      for (i2=i; i2<current_game->board->extended_height-1; i2++) {
	for (j=0; j<current_game->board->width; j++) {
	  current_fancy_board[i2][j]=current_fancy_board[i2+1][j];
	}
      }
      i-=1;    
    }
  }
  /* update fancy board 2 */
  for (i=0; i<current_game2->board->extended_height; i++) {
    line_to_remove=0;
    for (j=0; j<current_game2->board->width; j++) {
      if (current_fancy_board2[i][j]==-1) { /* there is a piece to put */
	current_fancy_board2[i][j]=current_color;	
      } else if (current_fancy_board2[i][j]==-2) { /* there is a line to remove */     
	line_to_remove=1;
      }      
    }
    if (line_to_remove) {
      for (i2=i; i2<current_game2->board->extended_height-1; i2++) {
	for (j=0; j<current_game2->board->width; j++) {
	  current_fancy_board2[i2][j]=current_fancy_board2[i2+1][j];
	}
      }
      i-=1;    
    }
  }
  
  /* checking for computer game over */
  if (current_game->board->wall_height > current_game->board->height) {
    current_game->game_over = 1;
  } else {
    generate_next_piece(current_game);
    current_pieceorientation = &current_game->current_piece->orientations[0];
    current_color=current_game->current_piece_index + 1;
  }

  /* checking for player game over */
  if (current_game2->board->wall_height > current_game2->board->height) {
    current_game2->game_over = 1;
  } else {
    if (current_game->game_over==1) { /* if the player is the only remaining player */
      generate_next_piece(current_game2);
      current_color=current_game2->current_piece_index + 1;
    } else {
      game_set_current_piece_index(current_game2,current_game->current_piece_index); /* synchronize pieces for both games */
    }
    column=(current_game->board->width-current_pieceorientation->width)/2;
    orientation=0;
    current_pieceorientation2 = &current_game2->current_piece->orientations[orientation];        
  }
  

  display_board_compete();       
  
  if ((current_game->game_over==1) && (current_game2->game_over==1)) {    /* if both have game over */

    /* end of game */    
    
  } else {     

    waiting = 0;
    
    if (current_game2->game_over==1) { /* if only the computer remains */
      glutTimerFunc(300, strategy_compete_only_computer, 0); /* call strategy_compete_only_computer() again in .3 second */
    } 
    
  }

}


void key_pressed_compete (unsigned char key, int x, int y) {

  Action action;
  int i,j;

  printf("keypressed:%d\n",key);

  if ((key==27) || (key=='Z')) { /* ESC/Z : restart game */

    if ((current_game->game_over==1) && (current_game2->game_over==1)) {
      nb_games++;
      tots1+=current_score;
      tots2+=current_score2;
    }
    game_reset(current_game);   
    game_reset(current_game2);   
    current_score=0;
    current_score2=0;
    game_set_current_piece_index(current_game2,current_game->current_piece_index); /* synchronize pieces for both games */
    current_pieceorientation = &current_game->current_piece->orientations[0];
    column=(current_game->board->width-current_pieceorientation->width)/2;
    orientation=0;
    current_pieceorientation2 = &current_game2->current_piece->orientations[orientation];
    current_color=current_game->current_piece_index + 1;
  
    for (i=0; i<current_game->board->extended_height; i++) {
      for (j=0; j<current_game->board->width; j++) {
	current_fancy_board[i][j]=0;
	current_fancy_board2[i][j]=0;
      }
    }      

    waiting=0;

    if (key=='Z') {
      nb_games=0;
      tots1=0;
      tots2=0;
    }
    display_board_compete();

  }

  if (waiting==0) {
    
    if (key==' ') {
      
      waiting=1;

      /* action of the computer */
      if (current_game->game_over != 1) {
	current_strategy->decide(current_game, &action);
	current_score += board_drop_piece_fancy(current_game->board, &current_game->current_piece->orientations[action.orientation],action.orientation, action.column,&current_game->last_move_info, 0, current_fancy_board);
	current_game->score=current_score;
	if (current_game->board->wall_height > current_game->board->height) {
	  current_game->game_over = 1;
	}
	if (current_strategy->info != NULL) {
	  if (current_game->game_over!=1) {
	    current_value=current_strategy->info(current_game);
	  }
	}
      }
      
      /* action of the human */
      if (current_game2->game_over != 1) {
	current_score2 += board_drop_piece_fancy(current_game2->board, &current_game2->current_piece->orientations[orientation],orientation, column+1 ,&current_game2->last_move_info, 0, current_fancy_board2);
	current_game2->score=current_score2;
      }
            
      display_board_compete();
      
      glutTimerFunc(300, strategy_compete_next, 0); /* call me again in .3 second */
    }

  }

}

void special_key_pressed_compete (int key, int x, int y) {
  
  int nb_or,nb_col;
  PieceOrientation *old_po;

  nb_or=game_get_nb_possible_orientations(current_game2);  
  
  printf("specialkeypressed:%d\n",key);

  if (waiting==0) {

    switch(key) {
    case GLUT_KEY_LEFT:
      column--;          
      break;
    case GLUT_KEY_RIGHT:
      column++;
      break;
    case GLUT_KEY_DOWN:  
      orientation=(orientation+nb_or-1)%nb_or;
      old_po=current_pieceorientation2;
      current_pieceorientation2 = &current_game2->current_piece->orientations[orientation];
      column+=(old_po->width-current_pieceorientation2->width)/2;
      break;
    case GLUT_KEY_UP:
      orientation=(orientation+1)%nb_or;
      old_po=current_pieceorientation2;
      current_pieceorientation2 = &current_game2->current_piece->orientations[orientation];
      column+=(old_po->width-current_pieceorientation2->width)/2;
      break;
      
    }    
    
    /* adjust the column if necessary */
    
    if (column<0) {
      column=0;
    }
    nb_col=game_get_nb_possible_columns(current_game2, orientation);
    if (column>=nb_col) {
      column=nb_col-1;
    }
    
    display_board_compete();

  }

}

/**
 * Launch a parallel game (computer + player)
 * 
 * @param strategy the strategy played by the computer
 * @param width board width
 * @param height board height
 * @param allow_lines_after_overflow 1 to enable the lines completion when the piece overflows
 * @param piece_file_name description of the pieces
 * @param piece_sequence a sequence of pieces (NULL to generate the pieces randomly)
 * @return the game score
 */
int strategy_compete(int tetris_implementation, Strategy *strategy, int width, int height,
			int allow_lines_after_overflow, const char *piece_file_name, int *piece_sequence) {
  
  int i;

  current_counter=0;
  current_step_counter=0;
  current_score=0;
  current_score2=0;

  current_strategy = strategy;

  current_game = new_game(tetris_implementation, width, height, allow_lines_after_overflow, piece_file_name, piece_sequence);
  current_game2 = new_game(tetris_implementation, width, height, allow_lines_after_overflow, piece_file_name, piece_sequence);

  game_set_current_piece_index(current_game2,current_game->current_piece_index); /* synchronize pieces for both games */

  if (current_strategy->initialize != NULL) {
    current_strategy->initialize(current_game);
  }
  CALLOC(current_fancy_board, int*, current_game->board->extended_height);
  CALLOC(current_fancy_board2, int*, current_game->board->extended_height);
  for (i=0; i<current_game->board->extended_height; i++) {
    CALLOC(current_fancy_board[i], int, width);
    CALLOC(current_fancy_board2[i], int, width);
  }

  CALLOC(text2,char,20);
  
  current_pieceorientation = &current_game->current_piece->orientations[0];
  column=(current_game->board->width-current_pieceorientation->width)/2;
  orientation=0;
  current_pieceorientation2 = &current_game2->current_piece->orientations[orientation];
  current_color=current_game->current_piece_index + 1;
    
  /* Launch the glut loop */

  waiting=0;
  glut_loop( 
	    current_game->board->width*60*2,(current_game->board->height+5)*60+36,
	    "Fête de la Science 2008",
	    display_board_compete,
	    NULL,0,0,
	    key_pressed_compete,
	    special_key_pressed_compete
	    );
  
  /************************/
  
  if (current_strategy->exit != NULL) {
    current_strategy->exit();
  }

  current_score=current_game->score;
  free_game(current_game);
  free_game(current_game2);

  FREE(text2);
  FREE(filename);

  return current_score;
}


/**
 * Plays an entire game with the given strategy function.
 * The moves are saved into a binary file.
 * This file is readable thanks to the program mdptetris_view_game.
 * @param file_name name of the file to write
 * @param strategy a strategy
 * @param width board width
 * @param height board height
 * @param allow_lines_after_overflow 1 to enable the lines completion when the piece overflows
 * @param piece_file_name description of the pieces
 * @param piece_sequence a sequence of pieces (NULL to generate the pieces randomly)
 * @return the game score
 */
int strategy_play_game_file(int tetris_implementation, const char *file_name, Strategy *strategy, int width, int height,
			    int allow_lines_after_overflow, const char *piece_file_name, int *piece_sequence) {
  Game *game;
  Action action;
  int n,score;
  FILE *out;

  out = fopen(file_name, "w");  

  initialize_interruptions();

  game = new_game(tetris_implementation, width, height, allow_lines_after_overflow, piece_file_name, piece_sequence);
  
  if (strategy->initialize != NULL) {
    strategy->initialize(game);
  }

  FWRITE(&width, sizeof(int), 1, out);
  FWRITE(&height, sizeof(int), 1, out);
  FWRITE(&allow_lines_after_overflow, sizeof(int), 1, out);
  n = strlen(piece_file_name) + 1;
  FWRITE(&n, sizeof(int), 1, out);
  FWRITE(piece_file_name, sizeof(char), n, out);

  do {
    FWRITE(&game->score, sizeof(int), 1, out);
    FWRITE(&game->current_piece_index, sizeof(int), 1, out);
    FWRITE(game->board->rows, sizeof(int), game->board->height, out);
    strategy->decide(game, &action);
    game_drop_piece(game, &action, 0);
  }
  while (!game->game_over && !is_interrupted());

  exit_interruptions();
  fclose(out);

  if (strategy->exit != NULL) {
    strategy->exit();
  }

  score=game->score;
  free_game(game);

  return score;
}

/**
 * Plays an entire game with the given strategy function.
 * @param strategy the strategy
 * @param game a reseted game
 * @return the game score
 */
int strategy_play_game(Strategy *strategy, Game *game) {
  Action action;

  do {
    strategy->decide(game, &action);
    game_drop_piece(game, &action, 0);
  }
  while (!game->game_over);

  return game->score;
}

/**
 * Plays a number of games with a strategy.
 */
void strategy_play_games(int tetris_implementation, Strategy *strategy, int nb_games, int width, int height,
			 int allow_lines_after_overflow, const char *piece_file_name, int *piece_sequence) {
  Game *game;
  int i, score;
  GamesStatistics *stats;

  game = new_game(tetris_implementation,width, height, allow_lines_after_overflow, piece_file_name, piece_sequence);
  
  if (strategy->initialize != NULL) {
    strategy->initialize(game);
  }

  initialize_interruptions();

  if (nb_games > 1) {
    printf("Playing %d games\n", nb_games);
  }
  else {
    printf("Playing 1 game\n");
  }

  stats = games_statistics_new(NULL, nb_games, NULL);

  for (i = 0; i < nb_games && !is_interrupted(); i++) {
    score = strategy_play_game(strategy, game);
    if (nb_games > 1 && !is_interrupted()) {
      printf("%d ",score);
      fflush(stdout);
    }
    
    games_statistics_add_game(stats, score);

    game_reset(game);
  }
  free_game(game);
  
  if (strategy->exit != NULL) {
    strategy->exit();
  }

  printf("\nMean score: %f\n", stats->mean);
  games_statistics_end_episode(stats, NULL);

  games_statistics_free(stats);

  exit_interruptions();
}
