#pragma once

#include "fixed_point.hpp"

//using namespace sg14;
//using namespace std;

// signed value with 15 integer and 16 fractional bits (s15:16)
typedef sg14::fixed_point<int32_t, -16> tGLfixed;
//typedef float tGLfixed;

// signed accumulator value with 31 integer and 32 fractional bits (s31:32)
typedef sg14::fixed_point<int64_t, -32> tGLfixed_acc;

// signed value with 15 integer and 16 fractional bits (s15:16)
//typedef sg14::fixed_point<int32_t, -16> {3.1415926535897932} GLfixed_pi;
const tGLfixed GLfixed_PI{3.1415926535897932};

