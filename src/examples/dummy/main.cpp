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
#include "x11.h"
#include "x11.hpp"

//#include <gles/vertex.hpp> // todo: move to api.h
//#include <gles/matrix.hpp> // todo: move to api.h
//#include <gles/light.hpp> // todo: move to api.h
//#include <gles/misc.hpp> // todo: move to api.h
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
static void gear( GLfixed inner_radius, GLfixed outer_radius, GLfixed width,
		  GLint teeth, GLfixed tooth_depth )
{
   GLint i;
   GLfixed r0, r1, r2;
   GLfixed angle, da;
   GLfixed u, v, len;

   r0 = inner_radius;
   r1 = outer_radius - tooth_depth/2.0;
   r2 = outer_radius + tooth_depth/2.0;

   da = 2.0*M_PI / teeth / 4.0;

   glShadeModel( GL_FLAT );

   const tGLfixed n[3] = {0.0, 0.0, 1.0};
   glNormal3xv( n );

   /* draw front face */
   glBegin( GL_POLYGON/*GL_QUAD_STRIP*/ );
   for (i=0;i<=teeth;i++) {
      angle = i * 2.0*M_PI / teeth;
      glVertex3x( r0*cos(angle), r0*sin(angle), width*0.5 );
      glVertex3x( r1*cos(angle), r1*sin(angle), width*0.5 );
      glVertex3x( r0*cos(angle), r0*sin(angle), width*0.5 );
      glVertex3x( r1*cos(angle+3*da), r1*sin(angle+3*da), width*0.5 );
   }
   glEnd();

}

int main(int argc, char **argv) 
{
    test_cpplink();
    test_clink();
    return ui_loop(argc, argv, "gears");
}
