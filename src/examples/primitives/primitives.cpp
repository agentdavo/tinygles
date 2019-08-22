/* triangle.c */

/*
 * Simplest example to draw flat triangle.  This program is in the public domain.
 */


#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../x11.hpp"

#include <gles/zgl.hpp>
#include <gles/api.hpp>
#include <gles/math_tests.hpp>

#include <iostream>
#include <math.h>

static tGLfixed gTime = 0.0;

static GLuint limit;
static GLuint count = 1;

static tGLfixed black[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; // ok
static tGLfixed red[4] = { 1.0f, 0.0f, 0.0f, 1.0f }; // ok
static tGLfixed green[4] = { 0.0f, 1.0f, 0.0f, 1.0f }; // ok
static tGLfixed blue[4] = { 0.0f, 0.0f, 1.0f, 1.0f }; // not ok, gives #ffffff

namespace fp {

void draw_triangle(int color_mode)
{
    static tGLfixed r = 0.1f;
    static tGLfixed z = 0.0f;

    static tGLfixed v0[3] = {-1.0*r, -1.0*r, z};
    static tGLfixed v1[3] = {1.0*r, -1.0*r, z};
    static tGLfixed v2[3] = {0.0f, 0.0f, z};

    fp::glBegin( GL_TRIANGLES );

    if (color_mode == 0) {
        fp::glColor3xv(red);   fp::glVertex3xv(v0);
        fp::glColor3xv(green); fp::glVertex3xv(v1);
        fp::glColor3xv(blue);  fp::glVertex3xv(v2);
    }
    else if (color_mode == 1) {
        fp::glColor3xv(red); fp::glVertex3xv(v0);
        fp::glColor3xv(red); fp::glVertex3xv(v1);
        fp::glColor3xv(red); fp::glVertex3xv(v2);
    }
    else if (color_mode == 2) {
        fp::glColor3xv(green); fp::glVertex3xv(v0);
        fp::glColor3xv(green); fp::glVertex3xv(v1);
        fp::glColor3xv(green); fp::glVertex3xv(v2);
    }
    else if (color_mode == 3) {
        fp::glColor3xv(blue); fp::glVertex3xv(v0);
        fp::glColor3xv(blue); fp::glVertex3xv(v1);
        fp::glColor3xv(blue); fp::glVertex3xv(v2);
    }

    fp::glColor3xv(blue); fp::glVertex3xv(v0);
    fp::glColor3xv(blue); fp::glVertex3xv(v1);
    fp::glColor3xv(blue); fp::glVertex3xv(v2);

    fp::glEnd();
}

void draw_lines(int color_mode)
{
    static tGLfixed r2 = 0.2f;
    static tGLfixed z = 0.0f;

    fp::glBegin( GL_LINES );
    if (color_mode == 1) {
        fp::glColor3xv(red);
    } else if (color_mode == 2) {
        fp::glColor3xv(green);
    } else if (color_mode == 3) {
        fp::glColor3xv(blue);
    }
    fp::glVertex3x(-r2, 0, z);
    fp::glVertex3x(r2, 0, z);
    fp::glVertex3x(0, -r2, z);
    fp::glVertex3x(0, r2, z);
    fp::glEnd();
}

void draw( void )
{
   fp::glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   fp::glClearColor4xv(black);

   // rotated triangle
   fp::glPushMatrix();

   fp::glTranslatex(-0.6, 0.6, 0);
   fp::glRotatex(gTime, 0, 0, 1);
   draw_triangle(1);

   fp::glPopMatrix();

   // draw scaled triangle
   fp::glPushMatrix();

   fp::glTranslatex(0.0, 0.6, 0);
   const tGLfixed s = 1.0f+0.5f*std::sin(0.02f*float(gTime));
   fp::glScalex(s, s, s);
   draw_triangle(2);

   fp::glPopMatrix();

   // draw movable triangle
   fp::glPushMatrix();

   const tGLfixed x = 0.6f+0.2f*std::sin(0.02f*float(gTime));
   fp::glTranslatex(x, 0.6, 0);
   draw_triangle(3);

   fp::glPopMatrix();

   // draw point at the center
   fp::glBegin( GL_POINTS );
   fp::glColor3xv(red);
   fp::glVertex3x(0, 0, 0);
   fp::glEnd();

   // draw rotated lines
   fp::glPushMatrix();
   fp::glTranslatex(-0.6, -0.6, 0);
   fp::glRotatex(gTime, 0, 0, 1);

   draw_lines(1);

   fp::glPopMatrix();


   // draw scaled lines
   fp::glPushMatrix();

   fp::glTranslatex(0.0, -0.6, 0);
   fp::glScalex(1.2*s, 1.2*s, 1.2*s);
   draw_lines(2);

   fp::glPopMatrix();

   // draw movable triangle
   fp::glPushMatrix();

   fp::glTranslatex(0.6, -x, 0);
   draw_lines(3);

   fp::glPopMatrix();

   fp::tkSwapBuffers();

   count++;
   if (count==limit) {
       exit(0);
   }
}



void idle( void )
{
   gTime += 0.5;
   draw();
}


/* change view angle, exit upon ESC */
GLenum key(int k, GLenum mask)
{
   return GL_FALSE;
}

/* new window size or exposure */
void reshape( int width, int height )
{
    std::cout<<"--- reshape"<<" w="<<width<<" h="<<height<<std::endl;
   //tGlfixed  h = (tGlfixed) height / (tGlfixed) width;

   fp::glViewport(0, 0, (GLint)width, (GLint)height);
   fp::glMatrixMode(GL_PROJECTION);
   fp::glLoadIdentity();
   //tGLfixed r = 20;
   //fp::glOrthox(0, r, 0, r, -10, 10);

   //fp::glFrustumx( -1.0, 1.0, -height, height, 5.0, 60.0 );

   fp::glMatrixMode(GL_MODELVIEW);
   fp::glLoadIdentity();

   {
       std::cout<<"read GL_PROJECTION_MATRIX"<<std::endl;
       tGLfixed buf[16];
       fp::glGetFloatv(GL_PROJECTION_MATRIX, buf);
       fp::gl_print_matrix2(buf);
       std::cout<<"---"<<std::endl;
   }
   {
       std::cout<<"read GL_MODELVIEW_MATRIX"<<std::endl;
       tGLfixed buf[16];
       fp::glGetFloatv(GL_MODELVIEW_MATRIX, buf);
       fp::gl_print_matrix2(buf);
       std::cout<<"---"<<std::endl;
   }
}

void init( void )
{
    fp::glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //runtests();
}

} // namespace fp


int main(int argc, char **argv)
{
   if (argc>1) {
      /* do 'n' frames then exit */
      limit = atoi( argv[1] ) + 1;
   }
   else {
      limit = 0;
   }

   return fp::ui_loop(argc, argv, "triangle");
}


