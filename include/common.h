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
#include <sys/time.h>

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

#define V2(X, Y) ((v2) {X, Y})
#define V3(X, Y, Z) ((v3) {X, Y, Z})
#define V4(X, Y, Z, W) ((v4) {X, Y, Z, W})
#define MAT4(A) mm_mat4d(A)

#define V3I(X, Y, Z) ((v3i) {X, Y, Z})
#define V3I_EQUAL(A, B) (A.x == B.x && A.y == B.y && A.z == B.z)
#define V3I_OFFSET(A, B) V3I(A.x - B.x, A.y - B.y, A.z - B.z)

typedef int64_t i64;
typedef uint64_t u64;
typedef int32_t i32;
typedef uint32_t u32;
typedef int16_t i16;
typedef uint16_t u16;
typedef int8_t i8;
typedef uint8_t u8;
typedef float r32;
typedef double r64;

typedef struct v4 {
  float x, y, z, w;
} v4;

typedef struct v3 {
  float x, y, z;
} v3;

typedef struct v2 {
  float x, y;
} v2;

typedef struct v3i {
  i32 x, y, z;
} v3i;

inline float random_number(float from, float to) {
  return (float)rand() / (float)(RAND_MAX / to) + (float)rand() / (float)(RAND_MAX / from);
}

extern FILE* log_file;

char* read_entire_file(const char* path);

#endif
