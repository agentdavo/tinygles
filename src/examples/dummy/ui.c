/* gears.c */

/*
 * 3-D gear wheels.  This program is in the public domain.
 *
 * Brian Paul
 */


#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <GL/glx.h> 
#include <GL/gl.h> 
#include "../ui.h"

static GLfixed view_rotx=20.0, view_roty=30.0, view_rotz=0.0;
static GLint gear1, gear2, gear3;
static GLfixed angle = 0.0;

static GLuint limit;
static GLuint count = 1;

void draw( void )
{
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

//   glPushMatrix();
//   glRotatex( view_rotx, 1.0, 0.0, 0.0 );
//   glRotatex( view_roty, 0.0, 1.0, 0.0 );
//   glRotatex( view_rotz, 0.0, 0.0, 1.0 );

//   glPushMatrix();
//   glTranslatex( -3.0, -2.0, 0.0 );
//   glRotatex( angle, 0.0, 0.0, 1.0 );
//   glCallList(gear1);
//   glPopMatrix();

//   glPushMatrix();
//   glTranslatex( 3.1, -2.0, 0.0 );
//   glRotatex( -2.0*angle-9.0, 0.0, 0.0, 1.0 );
//   glCallList(gear2);
//   glPopMatrix();

//   glPushMatrix();
//   glTranslatex( -3.1, 4.2, 0.0 );
//   glRotatex( -2.0*angle-25.0, 0.0, 0.0, 1.0 );
//   glCallList(gear3);
//   glPopMatrix();

//   glPopMatrix();

//   tkSwapBuffers();

//   count++;
//   if (count==limit) {
//       exit(0);
//   }
}



void idle( void )
{
   angle += 2.0;
   draw();
}



/* change view angle, exit upon ESC */
GLenum key(int k, GLenum mask)
{
   switch (k) {
      case KEY_UP:
         view_rotx += 5.0;
	 return GL_TRUE;
      case KEY_DOWN:
         view_rotx -= 5.0;
	 return GL_TRUE;
      case KEY_LEFT:
         view_roty += 5.0;
	 return GL_TRUE;
      case KEY_RIGHT:
         view_roty -= 5.0;
	 return GL_TRUE;
      case 'z':
	 view_rotz += 5.0;
	 return GL_TRUE;
      case 'Z':
	 view_rotz -= 5.0;
	 return GL_TRUE;
      case KEY_ESCAPE:
          exit(0);
   }
   return GL_FALSE;
}



/* new window size or exposure */
void reshape( int width, int height )
{
   GLfixed  h = (GLfixed) height / (GLfixed) width;

   glViewport(0, 0, (GLint)width, (GLint)height);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   //glFrustumx( -1.0, 1.0, -h, h, 5.0, 60.0 );
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   //glTranslatex( 0.0, 0.0, -40.0 );
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}


void init( void )
{
//   static tGLfixed pos[4] = {5.0, 5.0, 10.0, 0.0 };
//   static tGLfixed red[4] = {0.8, 0.1, 0.0, 1.0 };
//   static tGLfixed green[4] = {0.0, 0.8, 0.2, 1.0 };
//   static tGLfixed blue[4] = {0.2, 0.2, 1.0, 1.0 };

//   glLightxv( GL_LIGHT0, GL_POSITION, pos );
   glEnable( GL_CULL_FACE );
   glEnable( GL_LIGHTING );
   glEnable( GL_LIGHT0 );
   glEnable( GL_DEPTH_TEST );

   /* make the gears */
   gear1 = glGenLists(1);
   glNewList(gear1, GL_COMPILE);
   //glMaterialxv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red );
   //gear( 1.0, 4.0, 1.0, 20, 0.7 );
   glEndList();

   gear2 = glGenLists(1);
   glNewList(gear2, GL_COMPILE);
   //glMaterialxv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green );
   //gear( 0.5, 2.0, 2.0, 10, 0.7 );
   glEndList();

   gear3 = glGenLists(1);
   glNewList(gear3, GL_COMPILE);
   //glMaterialxv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue );
   //gear( 1.3, 2.0, 0.5, 10, 0.7 );
   glEndList();

   glEnable( GL_NORMALIZE );
}
