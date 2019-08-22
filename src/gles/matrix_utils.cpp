/* Some simple mathematical functions. Don't look for some logic in
   the function names :-) */

#include "matrix_utils.hpp"
//#include "api.hpp"

namespace fp {

M4* getM4(const tGLfixed& _m00, const tGLfixed& _m01, const tGLfixed& _m02, const tGLfixed& _m03,
          const tGLfixed& _m10, const tGLfixed& _m11, const tGLfixed& _m12, const tGLfixed& _m13,
          const tGLfixed& _m20, const tGLfixed& _m21, const tGLfixed& _m22, const tGLfixed& _m23,
          const tGLfixed& _m30, const tGLfixed& _m31, const tGLfixed& _m32, const tGLfixed& _m33)
{
    M4* m = new M4;
    m->m[0][0] = _m00;   m->m[0][1] = _m01;   m->m[0][2] = _m02;   m->m[0][3] = _m03;
    m->m[1][0] = _m10;   m->m[1][1] = _m11;   m->m[1][2] = _m12;   m->m[1][3] = _m13;
    m->m[2][0] = _m20;   m->m[2][1] = _m21;   m->m[2][2] = _m22;   m->m[2][3] = _m23;
    m->m[3][0] = _m30;   m->m[3][1] = _m31;   m->m[3][2] = _m32;   m->m[3][3] = _m33;
    return m;
}

M4* getM4(tGLfixed* arr)
{
    M4* m = new M4;
    fillMatrix(*m, arr);
    return m;
}

M4* getIdentity()
{
    return getM4(1, 0, 0, 0,
                 0, 1, 0, 0,
                 0, 0, 1, 0,
                 0, 0, 0, 1);

}

void fillMatrix(M4& m, tGLfixed* arr) {
    m.m[0][0] = arr[0];   m.m[0][1] = arr[1];   m.m[0][2] = arr[2];   m.m[0][3] = arr[3];
    m.m[1][0] = arr[4];   m.m[1][1] = arr[5];   m.m[1][2] = arr[6];   m.m[1][3] = arr[7];
    m.m[2][0] = arr[8];   m.m[2][1] = arr[9];   m.m[2][2] = arr[10];  m.m[2][3] = arr[11];
    m.m[3][0] = arr[12];  m.m[3][1] = arr[13];  m.m[3][2] = arr[14];  m.m[3][3] = arr[15];
}

} // namespace fp
