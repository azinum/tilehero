// matrix_math.h
// Based on HandMadeMath

#ifndef _MMATH_H
#define _MMATH_H

#include <math.h>

#undef USE_SSE // Streaming SIMD Extensions
#define PI_32 3.14159265359f


typedef struct vec3 {
	float x, y, z;
} vec3;

typedef struct quaternion {
  float x, y, z, w;
} quaternion;

typedef struct mat4 {
  float elements[4][4];
} mat4;

#define translate(M, X, Y) { \
  M = mm_multiply_mat4(M, mm_translate((vec3) {X, Y, 0})); \
} \

inline float mm_toradians(float degrees) {
  float result = degrees * (PI_32 / 180.0f);
  return result;
}

inline float mm_dot_vec3(vec3 a, vec3 b) {
  float result = (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
  return result;
}

inline vec3 mm_add_vec3(vec3 a, vec3 b) {
  vec3 result;

  result.x = a.x + b.x;
  result.y = a.y + b.y;
  result.z = a.z + b.z;

  return result;
}

inline vec3 mm_subtract_vec3(vec3 a, vec3 b) {
  vec3 result;

  result.x = a.x - b.x;
  result.y = a.y - b.y;
  result.z = a.z - b.z;

  return result;
}

inline vec3 mm_cross(vec3 a, vec3 b) {
  vec3 result;

  result.x = (a.y * b.z) - (a.z * b.y);
  result.y = (a.z * b.x) - (a.x * b.z);
  result.z = (a.x * b.y) - (a.y * b.x);

  return result;
}

inline float mm_length_sq_vec3(vec3 vec) {
  float result = mm_dot_vec3(vec, vec);
  return result;
}

inline float mm_length_vec3(vec3 vec) {
  float result = sqrtf(mm_length_sq_vec3(vec));
  return result;
}

inline vec3 mm_normalize_vec3(vec3 vec) {
  vec3 result = {0};
  float length = mm_length_vec3(vec);

  if (length != 0.0f) {
    result.x = vec.x * (1.0f / length);
    result.y = vec.y * (1.0f / length);
    result.z = vec.z * (1.0f / length);
  }
  return result;
}

inline mat4 mat4_zero(void) {
  mat4 result = {0};
  return result;
}

inline mat4 mm_perspective(float fov, float aspect_ratio, float z_near, float z_far) {
  mat4 result = mat4_zero();

  float tan_theta = tanf(fov * (PI_32 / 360.0f));
  result.elements[0][0] = 1.0f / tan_theta;
  result.elements[1][1] = aspect_ratio / tan_theta;
  result.elements[2][3] = -1.0f;
  result.elements[2][2] = (z_near + z_far) / (z_near - z_far);
  result.elements[3][2] = (2.0f * z_near * z_far) / (z_near - z_far);
  result.elements[3][3] = 1.0f; // Note: Can't see anything if this is 0. Need to check what it does.

  return result;
}

inline mat4 mm_orthographic(float left, float right, float bottom, float top, float z_near, float z_far) {
  mat4 result = mat4_zero();

  result.elements[0][0] = 2.0f / (right - left);
  result.elements[1][1] = 2.0f / (top - bottom);
  result.elements[2][2] = 2.0f / (z_near - z_far);
  result.elements[3][3] = 1.0f;

  result.elements[3][0] = (left + right) / (left - right);
  result.elements[3][1] = (bottom + top) / (bottom - top);
  result.elements[3][2] = (z_far + z_near) / (z_near - z_far);

  return result;
}

inline mat4 mm_mat4d(float diagonal) {
  mat4 result = mat4_zero();

  result.elements[0][0] = diagonal;
  result.elements[1][1] = diagonal;
  result.elements[2][2] = diagonal;
  result.elements[3][3] = diagonal;

  return result;
}

inline mat4 mm_multiply_mat4(mat4 a, mat4 b) {
  mat4 result;
#ifdef USE_SSE

#else
  int column;
  for (column = 0; column < 4; ++column) {
    int rows;
    for (rows = 0; rows < 4; ++rows) {
      float sum = 0;
      int current_matrice;
      for (current_matrice = 0; current_matrice < 4; ++current_matrice) {
        sum += a.elements[current_matrice][rows] * b.elements[column][current_matrice];
      }
      result.elements[column][rows] = sum;
    }
  }
#endif

  return result;
}

inline mat4 mm_rotate(float angle, vec3 axis) {
  mat4 result = mm_mat4d(1.0f);

  axis = mm_normalize_vec3(axis);

  float sin_theta = sinf(mm_toradians(angle));
  float cos_theta = cosf(mm_toradians(angle));
  float cos_value = 1.0f - cos_theta;

  result.elements[0][0] = (axis.x * axis.x * cos_value) + cos_theta;
  result.elements[0][1] = (axis.x * axis.y * cos_value) + (axis.z * sin_theta);
  result.elements[0][2] = (axis.x * axis.z * cos_value) - (axis.y * sin_theta);

  result.elements[1][0] = (axis.y * axis.x * cos_value) - (axis.z * sin_theta);
  result.elements[1][1] = (axis.y * axis.y * cos_value) + cos_theta;
  result.elements[1][2] = (axis.y * axis.z * cos_value) + (axis.x * sin_theta);

  result.elements[2][0] = (axis.z * axis.x * cos_value) + (axis.y * sin_theta);
  result.elements[2][1] = (axis.z * axis.y * cos_value) - (axis.x * sin_theta);
  result.elements[2][2] = (axis.z * axis.z * cos_value) + cos_theta;
  return result;
}

inline mat4 mm_lookat(vec3 eye, vec3 center, vec3 up) {
  mat4 result;

  vec3 f = mm_normalize_vec3(mm_subtract_vec3(center, eye));
  vec3 s = mm_normalize_vec3(mm_cross(f, up));
  vec3 u = mm_cross(s, f);

  result.elements[0][0] = s.x;
  result.elements[0][1] = u.x;
  result.elements[0][2] = -f.x;

  result.elements[1][0] = s.y;
  result.elements[1][1] = u.y;
  result.elements[1][2] = -f.y;

  result.elements[2][0] = s.z;
  result.elements[2][1] = u.z;
  result.elements[2][2] = -f.z;

  result.elements[3][0] = -mm_dot_vec3(s, eye);
  result.elements[3][1] = -mm_dot_vec3(u, eye);
  result.elements[3][2] = mm_dot_vec3(f, eye);
  result.elements[3][3] = 1.0f;

  return (result);
}

inline mat4 mm_translate(vec3 translation) {
  mat4 result = mm_mat4d(1.0f);

  result.elements[3][0] = translation.x;
  result.elements[3][1] = translation.y;
  result.elements[3][2] = translation.z;

  return result;
}

inline mat4 mm_translate_mat4(mat4 a, vec3 translation) {
  mat4 result = a;

  result.elements[3][0] = translation.x;
  result.elements[3][1] = translation.y;
  result.elements[3][2] = translation.z;

  return result;
}

#endif
