#ifndef GRAPHICS_H
#define GRAPHICS_H

extern int window_width,window_height;

/**
 * Function type for a display function
 */
typedef void (GraphicsDisplayFunction)(void);

/**
 * Function type for a keypress function
 */
typedef void (GraphicsKeyPressFunction)(unsigned char key, int x, int y);

/**
 * Function type for a special keypress function
 */
typedef void (GraphicsSpecialKeyPressFunction)(int key, int x, int y);

/**
 * Function type for a keypress function
 */
typedef void (GraphicsTimerFunction)(int value);



/* fonts for text */
static void *glutFonts[7] = { 
    GLUT_BITMAP_9_BY_15, 
    GLUT_BITMAP_8_BY_13, 
    GLUT_BITMAP_TIMES_ROMAN_10, 
    GLUT_BITMAP_TIMES_ROMAN_24, 
    GLUT_BITMAP_HELVETICA_10, 
    GLUT_BITMAP_HELVETICA_12, 
    GLUT_BITMAP_HELVETICA_18 
}; 

/**
 * General functions
 */
void window_draw_line(int x1,int y1,int x2,int y2);
void window_draw_rectangle(int x,int y,int dx,int dy);
void glutPrint(float x, float y, void * font, char* text, float r, float g, float b, float a);
void save_window_to_file(char *filename, int w , int h);
void glut_loop(int w,int h,char *title,GraphicsDisplayFunction display_function,GraphicsTimerFunction timer_function,int timer_delay,int timer_value, GraphicsKeyPressFunction keypress_function,  GraphicsSpecialKeyPressFunction special_function);  


#endif
