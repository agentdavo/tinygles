### TinyGLESfp - OpenGL ES 1.1 fixed point software renderer

Based on 
- TinyGLES (c) 2014 Ryan Hileman
- TinyGL 0.4 (c) 1997-2002 Fabrice Bellard


### General Description:

TinyGLESfp is a software OpenGL ES driver intended for use on our RISCV32 core 'oclui'.

      --------------------------------------------------------------
      | GLfixed  |  32  | signed 2's complement S15.16 scaled integer|
      --------------------------------------------------------------
      | GLclampx |  32  | S15.16 scaled integer clamped to [0, 1]    |
      --------------------------------------------------------------


### Fixed point library:

http://johnmcfarlane.github.io/fixed_point/


### tglAPI - GL ES 1.0 Fixed Point Compatibilty

 - [ ] glAlphaFuncx (GLenum func, GLclampx ref)
 - [ ] glClearColorx (GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha)
 - [ ] glClearDepthx (GLclampx depth)
 - [ ] glColor4x (GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha)
 - [ ] glDepthRangex (GLclampx n, GLclampx f)
 - [ ] glFogx (GLenum pname, GLfixed param)
 - [ ] glFogxv (GLenum pname, const GLfixed *param)
 - [ ] glFrustumx (GLfixed l, GLfixed r, GLfixed b, GLfixed t, GLfixed n, GLfixed f)
 - [ ] glLightModelx (GLenum pname, GLfixed param)
 - [ ] glLightModelxv (GLenum pname, const GLfixed *param)
 - [ ] glLightx (GLenum light, GLenum pname, GLfixed param)
 - [ ] glLightxv (GLenum light, GLenum pname, const GLfixed *params)
 - [ ] glLineWidthx (GLfixed width)
 - [ ] glLoadMatrixx (const GLfixed *m)
 - [ ] glMaterialx (GLenum face, GLenum pname, GLfixed param)
 - [ ] glMaterialxv (GLenum face, GLenum pname, const GLfixed *param)
 - [ ] glMultMatrixx (const GLfixed *m)
 - [ ] glMultiTexCoord4x (GLenum texture, GLfixed s, GLfixed t, GLfixed r, GLfixed q)
 - [ ] glNormal3x (GLfixed nx, GLfixed ny, GLfixed nz)
 - [ ] glOrthox (GLfixed l, GLfixed r, GLfixed b, GLfixed t, GLfixed n, GLfixed f)
 - [ ] glPointSizex (GLfixed size)
 - [ ] glPolygonOffsetx (GLfixed factor, GLfixed units)
 - [ ] glRotatex (GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
 - [ ] glSampleCoveragex (GLclampx value, GLboolean invert)
 - [ ] glScalex (GLfixed x, GLfixed y, GLfixed z)
 - [ ] glTexEnvx (GLenum target, GLenum pname, GLfixed param)
 - [ ] glTexEnvxv (GLenum target, GLenum pname, const GLfixed *params)
 - [ ] glTexParameterx (GLenum target, GLenum pname, GLfixed param)
 - [ ] glTranslatex (GLfixed x, GLfixed y, GLfixed z)


### tglAPI - GL ES 1.1 Fixed Point Compatibility

 - [ ] glClipPlanex (GLenum plane, const GLfixed *equation)
 - [ ] glGetClipPlanex (GLenum plane, GLfixed *equation)
 - [ ] glGetFixedv (GLenum pname, GLfixed *params)
 - [ ] glGetLightxv (GLenum light, GLenum pname, GLfixed *params)
 - [ ] glGetMaterialxv (GLenum face, GLenum pname, GLfixed *params)
 - [ ] glGetTexEnvxv (GLenum target, GLenum pname, GLfixed *params)
 - [ ] glGetTexParameterxv (GLenum target, GLenum pname, GLfixed *params)
 - [ ] glPointParameterx (GLenum pname, GLfixed param)
 - [ ] glPointParameterxv (GLenum pname, const GLfixed *params)
 - [ ] glPointSizePointerOES (GLenum type, GLsizei stride, const GLvoid *pointer)
 - [ ] glTexParameterxv (GLenum target, GLenum pname, const GLfixed *params)

Notes - limitations:
--------------------
