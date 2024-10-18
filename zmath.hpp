#pragma once

#include "fixed_point_type.hpp"
#include <string.h>

/* Matrix & Vertex */

namespace fp
{

     typedef struct alignas(16) M4
     {
          tGLfixed m[4][4];
     } M4;

     typedef struct alignas(16) M3
     {
          tGLfixed m[3][3];
     } M3;

     typedef struct alignas(16) M34
     {
          tGLfixed m[3][4];
     } M34;

     struct alignas(16) V3
     {
          tGLfixed X, Y, Z;

          // Constructor for V3
          V3(tGLfixed x = 0, tGLfixed y = 0, tGLfixed z = 0)
              : X(x), Y(y), Z(z) {}
     };

     struct alignas(16) V4
     {
          tGLfixed X, Y, Z, W;

          // Constructor for V4
          V4(tGLfixed x = 0, tGLfixed y = 0, tGLfixed z = 0, tGLfixed w = 0)
              : X(x), Y(y), Z(z), W(w) {}
     };

     /* Function declarations */

     void gl_M4_Id(M4 *a);
     int gl_M4_IsId(M4 *a);
     void gl_M4_Move(M4 *a, M4 *b);
     void gl_MoveV3(V3 *a, V3 *b);
     void gl_MulM4V3(V3 *a, M4 *b, V3 *c);
     void gl_MulM3V3(V3 *a, M4 *b, V3 *c);

     void gl_M4_MulV4(V4 *a, M4 *b, V4 *c);
     void gl_M4_InvOrtho(M4 *a, M4 b);
     void gl_M4_Inv(M4 *a, M4 *b);
     void gl_M4_Mul(M4 *c, M4 *a, M4 *b);
     void gl_M4_MulLeft(M4 *c, M4 *a);
     void gl_M4_Transpose(M4 *a, M4 *b);
     void gl_M4_Rotate(M4 *c, tGLfixed t, int u);
     int gl_V3_Norm(V3 *a);

     V3 gl_V3_New(tGLfixed x, tGLfixed y, tGLfixed z);
     V4 gl_V4_New(tGLfixed x, tGLfixed y, tGLfixed z, tGLfixed w);

     int gl_Matrix_Inv(tGLfixed *r, tGLfixed *m, int n);

} // namespace fp
