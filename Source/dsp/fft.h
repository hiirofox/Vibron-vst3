#pragma once

#ifndef _MY_COMPLEX_
#define _MY_COMPLEX_

#include <math.h>
#include "complex.h"

#define M_PI 3.1415926535897932384626

void fft_f32(complex_f32_t* a, int n, int inv);


#endif