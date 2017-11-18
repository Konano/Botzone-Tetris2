#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>
#include "graphics.h"
#include "macros.h"

int window_width,window_height;

void window_reshape(int w, int h) {
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w, 0, h, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  window_width = w;
  window_height = h;
}



/* print text */
void glutPrint(float x, float y, void* font, char* text, float r, float g, float b, float a)  { 
  int blending = 0; 
  
  if(!text || !strlen(text)) return; 
  
  if(glIsEnabled(GL_BLEND)) blending = 1; 
  glEnable(GL_BLEND); 
  glColor4f(r,g,b,a); 
  glRasterPos2f(x,y); 
  while (*text) { 
    glutBitmapCharacter(font, *text); 
    text++; 
  } 
  if(blending==0) glDisable(GL_BLEND); 
} 

void window_draw_line(int x1,int y1,int x2,int y2) {
  
  glBegin(GL_LINES);
  glVertex2i(x1, y1);   glVertex2i(x2, y2);
  glEnd();

}

void window_draw_rectangle(int x,int y,int dx,int dy) {
  
  glBegin(GL_QUADS);
  glVertex2i(x, y);   glVertex2i(x, y+dy);
  glVertex2i(x+dx, y+dy); glVertex2i(x+dx, y);
  glEnd();

}


void save_window_to_file(char *filename, int w , int h) {

  int i;
  int linesize=3*w*sizeof(char);
  
  char *buffer = (char *) malloc (h*linesize);
  char *buffer2 = (char*) malloc (h*linesize);
  
  FILE *file;

  glPixelStorei (GL_PACK_ALIGNMENT, 1);
  glReadPixels (0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, buffer);
    
  /* invert the order of the lines */
  for (i=0; i<h; i++) {
    memcpy(buffer2+linesize*i,buffer+linesize*(h-1-i),linesize);
  }

  file = fopen (filename, "wb");
  fprintf (file, "P6 %d %d 255\n", w, h);
  FWRITE (buffer2, sizeof(GLubyte), w * h * 3, file);
  fclose (file);
  
  free(buffer);
  free(buffer2);
}

void glut_loop(int w,int h,char *title, GraphicsDisplayFunction display_function,GraphicsTimerFunction timer_function,int timer_delay,int timer_value, GraphicsKeyPressFunction keypress_function, GraphicsSpecialKeyPressFunction special_function) {
  
  int argcp=1; 
  window_width=w;
  window_height=h;
  
  glutInit(&argcp, NULL);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  if (! glutGet(GLUT_DISPLAY_MODE_POSSIBLE))
     glutInitDisplayMode(GLUT_INDEX);
  glutInitWindowSize (window_width,window_height);
  glutCreateWindow(title);
  if (keypress_function!=NULL)
    glutKeyboardFunc(keypress_function);
  if (special_function!=NULL)
    glutSpecialFunc(special_function);
  if (display_function!=NULL) 
    glutDisplayFunc(display_function);
  if (timer_function!=NULL) 
     glutTimerFunc(timer_delay, timer_function, timer_value); 

  glutReshapeFunc(window_reshape);

  glutMainLoop();

}



/* example window_keypress function */

void window_keypress_example (unsigned char key, int x, int y) {
  if (key == ' ')
    save_window_to_file("screenshot.pbm", window_width, window_height);
}


/* example window_display function */

void window_display_example(void) {

  glClearColor (1,1,1,1);
  glClear (GL_COLOR_BUFFER_BIT);
  
  glColor3f(1,0,0);
  /*    glPolygonMode (GL_FRONT_AND_BACK,GL_LINE);*/
  glPolygonMode (GL_FRONT_AND_BACK,GL_FILL); /* fill polygons */
  
  glPushMatrix();
  
  window_draw_rectangle(10,10,30,30);
  
  printf("%p\n",glutFonts[0]);

  glPopMatrix();
  
  glFlush();

}




/* int main(int argcp, char **argv) { */

/*   glut_loop(256,256,"Example",window_display_example,NULL,0,0,window_keypress_example); */

/*   return(0); */
  
/* } */
