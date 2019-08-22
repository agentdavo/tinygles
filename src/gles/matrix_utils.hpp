#pragma once

#include "matrix.hpp"

namespace fp {

M4* getM4(const tGLfixed& _m00, const tGLfixed& _m01, const tGLfixed& _m02, const tGLfixed& _m03,
          const tGLfixed& _m10, const tGLfixed& _m11, const tGLfixed& _m12, const tGLfixed& _m13,
          const tGLfixed& _m20, const tGLfixed& _m21, const tGLfixed& _m22, const tGLfixed& _m23,
          const tGLfixed& _m30, const tGLfixed& _m31, const tGLfixed& _m32, const tGLfixed& _m33);

M4* getM4(tGLfixed* arr);

M4* getIdentity();

void fillMatrix(M4& m, tGLfixed* arr);

} // namespace fp
