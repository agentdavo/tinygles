#pragma once

#include "fixed_point.hpp"
#include <cstdint>

// Signed value with 15 integer and 16 fractional bits (s15:16)
typedef sg14::fixed_point<int32_t, -16> tGLfixed;

// Signed accumulator value with 31 integer and 32 fractional bits (s31:32)
typedef sg14::fixed_point<int64_t, -32> tGLfixed_acc;

// Constant for Pi in fixed-point representation (s15:16)
const tGLfixed GLfixed_PI = sg14::fixed_point<int32_t, -16>(3.1415926535897932);
