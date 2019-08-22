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
#include "../x11.hpp"

#include <gles/vertex.hpp> // todo: move to api.h
#include <gles/matrix.hpp> // todo: move to api.h
#include <gles/light.hpp> // todo: move to api.h
#include <gles/misc.hpp> // todo: move to api.h
#include <gles/api.hpp>

//#define M_PI 3.14159265358979

/*
 * Draw a gear wheel.  You'll probably want to call this function when
 * building a display list since we do a lot of trig here.
 *
 * Input:  inner_radius - radius of hole at center
 *         outer_radius - radius at center of teeth
 *         width - width of gear
 *         teeth - number of teeth
 *         tooth_depth - depth of tooth
 */
static void gear( tGLfixed inner_radius, tGLfixed outer_radius, tGLfixed width,
          GLint teeth, tGLfixed tooth_depth )
{
   GLint i;
   tGLfixed r0, r1, r2;
   tGLfixed angle, da;
   tGLfixed u, v, len;

   r0 = inner_radius;
   r1 = outer_radius - tooth_depth/2.0;
   r2 = outer_radius + tooth_depth/2.0;

   da = 2.0*M_PI / teeth / 4.0;

   //glShadeModel( GL_FLAT );

   // todo:
//   const tGLfixed n[3] = {0.0, 0.0, 1.0};
//   fp::glNormal3xv( n );
//   fp::glColor3x(1.0, 1.0, 1.0);

   /* draw triangle */
//   fp::glBegin( GL_TRIANGLES );
//   tGLfixed r = 10;
//   fp::glVertex3x( -0.5*r, 0.0, -1.0 );
//   fp::glVertex3x( 0.5*r, 0.0, -1.0  );
//   fp::glVertex3x( 0.0, 0.5*r, -1.0  );
//   fp::glEnd();

//   /* draw front face */
//   //fp::glBegin( GL_POLYGON/*GL_QUAD_STRIP*/ );
//   //fp::glBegin( GL_QUAD_STRIP ); // todo: GL_QUAD_STRIP is not supported?
//   fp::glBegin( GL_TRIANGLES );
//   for (i=0;i<=teeth;i++) {
//      angle = i * 2.0*M_PI / teeth;
//      fp::glVertex3x( r0*cos(angle), r0*sin(angle), width*0.5 );
//      fp::glVertex3x( r1*cos(angle), r1*sin(angle), width*0.5 );
//      fp::glVertex3x( r0*cos(angle), r0*sin(angle), width*0.5 );
//      //fp::glVertex3x( r1*cos(angle+3*da), r1*sin(angle+3*da), width*0.5 );
//   }
//   fp::glEnd();

//   /* draw front sides of teeth */
//   //fp::glBegin( GL_QUADS ); todo: GL_QUADS is not supported?
//   fp::glBegin( GL_TRIANGLES );
//   da = 2.0*M_PI / teeth / 4.0;
//   for (i=0;i<teeth;i++) {
//      angle = i * 2.0*M_PI / teeth;

//      fp::glVertex3x( r1*cos(angle),      r1*sin(angle),      width*0.5 );
//      fp::glVertex3x( r2*cos(angle+da),   r2*sin(angle+da),   width*0.5 );
//      fp::glVertex3x( r2*cos(angle+2*da), r2*sin(angle+2*da), width*0.5 );
//      //fp::glVertex3x( r1*cos(angle+3*da), r1*sin(angle+3*da), width*0.5 );
//   }
//   fp::glEnd();


//   glNormal3x( 0.0, 0.0, -1.0 );

//   /* draw back face */
//   glBegin( GL_QUAD_STRIP );
//   for (i=0;i<=teeth;i++) {
//      angle = i * 2.0*M_PI / teeth;
//      glVertex3x( r1*cos(angle), r1*sin(angle), -width*0.5 );
//      glVertex3x( r0*cos(angle), r0*sin(angle), -width*0.5 );
//      glVertex3x( r1*cos(angle+3*da), r1*sin(angle+3*da), -width*0.5 );
//      glVertex3x( r0*cos(angle), r0*sin(angle), -width*0.5 );
//   }
//   glEnd();

//   /* draw back sides of teeth */
//   glBegin( GL_QUADS );
//   da = 2.0*M_PI / teeth / 4.0;
//   for (i=0;i<teeth;i++) {
//      angle = i * 2.0*M_PI / teeth;

//      glVertex3x( r1*cos(angle+3*da), r1*sin(angle+3*da), -width*0.5 );
//      glVertex3x( r2*cos(angle+2*da), r2*sin(angle+2*da), -width*0.5 );
//      glVertex3x( r2*cos(angle+da),   r2*sin(angle+da),   -width*0.5 );
//      glVertex3x( r1*cos(angle),      r1*sin(angle),      -width*0.5 );
//   }
//   glEnd();


//   /* draw outward faces of teeth */
//   glBegin( GL_QUAD_STRIP );
//   for (i=0;i<teeth;i++) {
//      angle = i * 2.0*M_PI / teeth;

//      glVertex3x( r1*cos(angle),      r1*sin(angle),       width*0.5 );
//      glVertex3x( r1*cos(angle),      r1*sin(angle),      -width*0.5 );
//      u = r2*cos(angle+da) - r1*cos(angle);
//      v = r2*sin(angle+da) - r1*sin(angle);
//      len = sqrt( u*u + v*v );
//      u /= len;
//      v /= len;
//      glNormal3x( v, -u, 0.0 );
//      glVertex3x( r2*cos(angle+da),   r2*sin(angle+da),    width*0.5 );
//      glVertex3x( r2*cos(angle+da),   r2*sin(angle+da),   -width*0.5 );
//      glNormal3x( cos(angle), sin(angle), 0.0 );
//      glVertex3x( r2*cos(angle+2*da), r2*sin(angle+2*da),  width*0.5 );
//      glVertex3x( r2*cos(angle+2*da), r2*sin(angle+2*da), -width*0.5 );
//      u = r1*cos(angle+3*da) - r2*cos(angle+2*da);
//      v = r1*sin(angle+3*da) - r2*sin(angle+2*da);
//      glNormal3x( v, -u, 0.0 );
//      glVertex3x( r1*cos(angle+3*da), r1*sin(angle+3*da),  width*0.5 );
//      glVertex3x( r1*cos(angle+3*da), r1*sin(angle+3*da), -width*0.5 );
//      glNormal3x( cos(angle), sin(angle), 0.0 );
//   }

//   glVertex3x( r1*cos(0), r1*sin(0), width*0.5 );
//   glVertex3x( r1*cos(0), r1*sin(0), -width*0.5 );

//   glEnd();


//   glShadeModel( GL_SMOOTH );

//   /* draw inside radius cylinder */
//   glBegin( GL_QUAD_STRIP );
//   for (i=0;i<=teeth;i++) {
//      angle = i * 2.0*M_PI / teeth;
//      glNormal3x( -cos(angle), -sin(angle), 0.0 );
//      glVertex3x( r0*cos(angle), r0*sin(angle), -width*0.5 );
//      glVertex3x( r0*cos(angle), r0*sin(angle), width*0.5 );
//   }
//   glEnd();
      
}


static GLfixed view_rotx=20.0, view_roty=30.0, view_rotz=0.0;
static GLint gear1, gear2, gear3;
static GLfixed angle = 0.0;

static GLuint limit;
static GLuint count = 1;

using namespace fp;

void draw( void )
{
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   fp::glBegin(GL_TRIANGLES);
   glColor3f( 1, 0, 0 ); // red
   glVertex2f( -0.8, -0.8 );
   glColor3f( 0, 1, 0 ); // green
   glVertex2f( 0.8, -0.8 );
   glColor3f( 0, 0, 1 ); // blue
   glVertex2f( 0, 0.9 );
   fp::glEnd();

   // todo:
//   fp::glPushMatrix();
//   glRotatex( view_rotx, 1.0, 0.0, 0.0 );
//   glRotatex( view_roty, 0.0, 1.0, 0.0 );
//   glRotatex( view_rotz, 0.0, 0.0, 1.0 );

//   fp::glPushMatrix();
//   glTranslatex( -3.0, -2.0, 0.0 );
//   glRotatex( angle, 0.0, 0.0, 1.0 );
//   glCallList(gear1);
//   fp::glPopMatrix();

//   fp::glPushMatrix();
//   glTranslatex( 3.1, -2.0, 0.0 );
//   glRotatex( -2.0*angle-9.0, 0.0, 0.0, 1.0 );
//   glCallList(gear2);
//   fp::glPopMatrix();

//   fp::glPushMatrix();
//   glTranslatex( -3.1, 4.2, 0.0 );
//   glRotatex( -2.0*angle-25.0, 0.0, 0.0, 1.0 );
//   glCallList(gear3);
//   fp::glPopMatrix();

//   fp::glPopMatrix();

   fp::tkSwapBuffers();

   count++;
   if (count==limit) {
       exit(0);
   }
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
   tGLfixed h = (tGLfixed) height / (tGLfixed) width;

   fp::glViewport(0, 0, (GLint)width, (GLint)height);
   fp::glMatrixMode(GL_PROJECTION);
   fp::glLoadIdentity();
   //glFrustumx( -1.0, 1.0, -h, h, 5.0, 60.0 );
   glOrtho(-1, 1, -1, 1, 1, -1);
    fp::glMatrixMode(GL_MODELVIEW);
   fp::glLoadIdentity();
   //glTranslatex( 0.0, 0.0, -40.0 );
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}


void init( void )
{
   static tGLfixed pos[4] = {5.0, 5.0, 10.0, 0.0 };
   static tGLfixed red[4] = {0.8, 0.1, 0.0, 1.0 };
   static tGLfixed green[4] = {0.0, 0.8, 0.2, 1.0 };
   static tGLfixed blue[4] = {0.2, 0.2, 1.0, 1.0 };

   // todo: cause error
   //glLightxv( GL_LIGHT0, GL_POSITION, pos );
   // todo
//   glEnable( GL_CULL_FACE );
//   glEnable( GL_LIGHTING );
//   glEnable( GL_LIGHT0 );
//   glEnable( GL_DEPTH_TEST );

   /* make the gears */
   gear1 = glGenLists(1);
   glNewList(gear1, GL_COMPILE);
   // todo: cause crash
   //glMaterialxv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red );
   gear( 1.0, 4.0, 1.0, 20, 0.7 );
   glEndList();

   gear2 = glGenLists(1);
   glNewList(gear2, GL_COMPILE);
   //todo: cause crash
   // glMaterialxv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green );
   gear( 0.5, 2.0, 2.0, 10, 0.7 );
   glEndList();

   gear3 = glGenLists(1);
   glNewList(gear3, GL_COMPILE);
   // todo: cause crash
   // glMaterialxv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue );
   gear( 1.3, 2.0, 0.5, 10, 0.7 );
   glEndList();

   glEnable( GL_NORMALIZE );
}

int main(int argc, char **argv) 
{
   if (argc>1) {
      /* do 'n' frames then exit */
      limit = atoi( argv[1] ) + 1;
   }
   else {
      limit = 0;
   }

   return fp::ui_loop(argc, argv, "gears");
}
