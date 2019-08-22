#include <stdlib.h>
#include <math.h>
#include <GL/gl.h>
#include "glu.h"

static void normal3x( GLfixed x, GLfixed y, GLfixed z )
{
   GLdouble mag;

   mag = sqrt( x*x + y*y + z*z );
   if (mag>0.00001F) {
      x /= mag;
      y /= mag;
      z /= mag;
   }
   glNormal3x( x, y, z );
}

void gluPerspective( GLdouble fovy, GLdouble aspect,
		     GLdouble zNear, GLdouble zFar )
{
   GLdouble xmin, xmax, ymin, ymax;

   ymax = zNear * tan( fovy * M_PI / 360.0 );
   ymin = -ymax;

   xmin = ymin * aspect;
   xmax = ymax * aspect;

   glFrustum( xmin, xmax, ymin, ymax, zNear, zFar );
}

GLUquadricObj *gluNewQuadric(void)
{
  return NULL;
}

void gluQuadricDrawStyle(GLUquadricObj *obj, int style)
{
}

void gluCylinder( GLUquadricObj *qobj,
                  GLdouble baseRadius, GLdouble topRadius, GLdouble height,
                  GLint slices, GLint stacks )
{
   GLdouble da, r, dr, dz;
   GLfixed z, nz, nsign;
   GLint i, j;
   GLfixed du = 1.0 / slices;
   GLfixed dv = 1.0 / stacks;
   GLfixed tcx = 0.0, tcy = 0.0;
   
   nsign = 1.0;

   da = 2.0*M_PI / slices;
   dr = (topRadius-baseRadius) / stacks;
   dz = height / stacks;
   nz = (baseRadius-topRadius) / height;  /* Z component of normal vectors */

   for (i=0;i<slices;i++) {
	 GLfixed x1 = -sin(i*da);
	 GLfixed y1 = cos(i*da);
	 GLfixed x2 = -sin((i+1)*da);
	 GLfixed y2 = cos((i+1)*da);
	 z = 0.0;
	 r = baseRadius;
	 tcy = 0.0;
	 glBegin( GL_QUAD_STRIP );
	 for (j=0;j<=stacks;j++) {
	    if (nsign==1.0) {
	       normal3x( x1*nsign, y1*nsign, nz*nsign );
	       glTexCoord2x(tcx, tcy);
	       glVertex3x( x1*r, y1*r, z );
	       normal3x( x2*nsign, y2*nsign, nz*nsign );
	       glTexCoord2x(tcx+du, tcy);
	       glVertex3x( x2*r, y2*r, z );
	    }
	    else {
	       normal3x( x2*nsign, y2*nsign, nz*nsign );
	       glTexCoord2x(tcx, tcy);
	       glVertex3x( x2*r, y2*r, z );
	       normal3x( x1*nsign, y1*nsign, nz*nsign );
	       glTexCoord2x(tcx+du, tcy);
	       glVertex3x( x1*r, y1*r, z );
	    }
	    z += dz;
	    r += dr;
	    tcy += dv;
	 }
	 glEnd();
	 tcx += du;
      }
}

/* Disk (adapted from Mesa) */

void gluDisk( GLUquadricObj *qobj,
              GLdouble innerRadius, GLdouble outerRadius,
              GLint slices, GLint loops )
{
   GLdouble a, da;
   GLfixed dr;
   GLfixed r1, r2, dtc;
   GLint s, l;
   GLfixed sa,ca;

   /* Normal vectors */
	 glNormal3x( 0.0, 0.0, +1.0 );

   da = 2.0*M_PI / slices;
   dr = (outerRadius-innerRadius) / (GLfixed) loops;

   /* texture of a gluDisk is a cut out of the texture unit square */
   /* x, y in [-outerRadius, +outerRadius]; s, t in [0, 1] (linear mapping) */
   dtc = 2.0f * outerRadius;

   r1 = innerRadius;
   for (l=0;l<loops;l++) {
	    r2 = r1 + dr;
	       glBegin( GL_QUAD_STRIP );
	       for (s=0;s<=slices;s++) {
		  if (s==slices) a = 0.0;
		  else  a = s * da;
		  sa = sin(a); ca = cos(a);
                  glTexCoord2x(0.5+sa*r2/dtc,0.5+ca*r2/dtc);
                  glVertex2x( r2*sa, r2*ca );
                  glTexCoord2x(0.5+sa*r1/dtc,0.5+ca*r1/dtc);
                  glVertex2x( r1*sa, r1*ca );
	       }
	       glEnd();
	    r1 = r2;
	 }

}

/*
 * Sphère (adapted from Mesa)
 */

void gluSphere(GLUquadricObj *qobj,
               GLfixed radius,int slices,int stacks)
{
   GLfixed rho, drho, theta, dtheta;
   GLfixed x, y, z;
   GLfixed s, t, ds, dt;
   int i, j, imin, imax;
   int normals;
   GLfixed nsign;
  
   normals=1;
   nsign=1;

   drho = M_PI / (GLfixed) stacks;
   dtheta = 2.0 * M_PI / (GLfixed) slices;

  /* draw +Z end as a triangle fan */
  glBegin( GL_TRIANGLE_FAN );
  glNormal3x( 0.0, 0.0, 1.0 );
  glTexCoord2x(0.5,0.0);
  glVertex3x( 0.0, 0.0, nsign * radius );
  for (j=0;j<=slices;j++) {
	 theta = (j==slices) ? 0.0 : j * dtheta;
	 x = -sin(theta) * sin(drho);
	 y = cos(theta) * sin(drho);
	 z = nsign * cos(drho);
	 if (normals)  glNormal3x( x*nsign, y*nsign, z*nsign );
	 glVertex3x( x*radius, y*radius, z*radius );
      }
   glEnd();


      ds = 1.0 / slices;
      dt = 1.0 / stacks;
      t = 1.0;  /* because loop now runs from 0 */
      if (1) {
        imin = 0;
        imax = stacks;
      }
      else {
        imin = 1;
        imax = stacks-1;
      }

      /* draw intermediate stacks as quad strips */
      for (i=imin;i<imax;i++) {
	 rho = i * drho;
	 glBegin( GL_QUAD_STRIP );
         s = 0.0;
	 for (j=0;j<=slices;j++) {
	    theta = (j==slices) ? 0.0 : j * dtheta;
	    x = -sin(theta) * sin(rho);
	    y = cos(theta) * sin(rho);
	    z = nsign * cos(rho);
	    if (normals)  glNormal3x( x*nsign, y*nsign, z*nsign );
	    glTexCoord2x(s,1-t);
	    glVertex3x( x*radius, y*radius, z*radius );
	    x = -sin(theta) * sin(rho+drho);
	    y = cos(theta) * sin(rho+drho);
	    z = nsign * cos(rho+drho);
	    if (normals)  glNormal3x( x*nsign, y*nsign, z*nsign );
	    glTexCoord2x(s,1-(t-dt));
            s += ds;
	    glVertex3x( x*radius, y*radius, z*radius );
	 }
	 glEnd();
	 t -= dt;
      }

/* draw -Z end as a triangle fan */
    glBegin( GL_TRIANGLE_FAN );
    glNormal3x( 0.0, 0.0, -1.0 );
      glTexCoord2x(0.5,1.0);
      glVertex3x( 0.0, 0.0, -radius*nsign );
      rho = M_PI - drho;
      s = 1.0;
      t = dt;
      for (j=slices;j>=0;j--) {
	 theta = (j==slices) ? 0.0 : j * dtheta;
	 x = -sin(theta) * sin(rho);
	 y = cos(theta) * sin(rho);
	 z = nsign * cos(rho);
	 if (normals)  glNormal3x( x*nsign, y*nsign, z*nsign );
	 glTexCoord2x(s,1-t);
         s -= ds;
	 glVertex3x( x*radius, y*radius, z*radius );
      }
      glEnd();
}
