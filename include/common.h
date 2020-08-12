// common.h

#ifndef _COMMON_H
#define _COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define NO_SSE

#ifndef NO_SSE

#if __SSE__
#define USE_SSE 1
#endif

#endif

#if USE_SSE
#include <xmmintrin.h>
#endif

#define PI32 3.14159265359f
#define ARR_SIZE(ARR) ((sizeof(ARR)) / (sizeof(ARR[0])))
#define MAX_PATH_LENGTH (512)

#define VEC2(X, Y) ((vec2) {X, Y})
#define VEC3(X, Y, Z) ((vec3) {X, Y, Z})
#define VEC4(X, Y, Z, W) ((vec4) {X, Y, Z, W})
#define MAT4(A) mm_mat4d(A)

typedef int64_t i64;
typedef uint64_t u64;
typedef int32_t i32;
typedef uint32_t u32;
typedef int16_t i16;
typedef uint16_t u16;
typedef int8_t i8;
typedef uint8_t u8;

inline float random_number(float from, float to) {
  return (float)rand() / (float)(RAND_MAX / to) + (float)rand() / (float)(RAND_MAX / from);
}

#endif
