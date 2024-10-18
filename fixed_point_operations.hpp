#pragma once

#include "fixed_point_type.hpp"

namespace fp {

tGLfixed fabs(const tGLfixed& val);

// this function is temprorary to solve issue while the tGLfixed1 *= tGLfixed2
tGLfixed multiply(const tGLfixed& a, const tGLfixed& b);

tGLfixed operator*(const tGLfixed& a1, const tGLfixed& a2);

//tGLfixed operator*=(tGLfixed& a1, const tGLfixed& a2);

} // namespace fp



