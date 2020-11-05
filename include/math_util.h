// math_util.h
// Thanks to HandMadeMath

#ifndef _MATH_UTIL_H
#define _MATH_UTIL_H

typedef union mat4 {
  float elements[4][4];
#if USE_SSE
  __m128 rows[4];
#endif
} mat4;

#define translate2d(MODEL, X, Y) { \
  MODEL = mm_multiply_mat4(MODEL, mm_translate((vec3) {X, Y, 0})); \
}
#define scale2d(MODEL, X, Y) { \
  MODEL = mm_multiply_mat4(MODEL, mm_scale((vec3) {X, Y, 1})); \
}
#define rotate2d(MODEL, ANGLE) { \
  MODEL = mm_multiply_mat4(MODEL, mm_rotate(ANGLE, (vec3) {0, 0, 1})); \
}

#ifdef USE_SSE

inline mat4 mm_transpose(mat4 a) {
  mat4 result = a;

  _MM_TRANSPOSE4_PS(result.rows[0], result.rows[1], result.rows[2], result.rows[3]);

  return result;
}

inline __m128 mm_linear_combine_sse(__m128 left, mat4 right) {
  __m128 result;

  result = _mm_mul_ps(_mm_shuffle_ps(left, left, 0x00), right.rows[0]);
  result = _mm_add_ps(result, _mm_mul_ps(_mm_shuffle_ps(left, left, 0x55), right.rows[1]));
  result = _mm_add_ps(result, _mm_mul_ps(_mm_shuffle_ps(left, left, 0xaa), right.rows[2]));
  result = _mm_add_ps(result, _mm_mul_ps(_mm_shuffle_ps(left, left, 0xff), right.rows[3]));

  return result;
}

#endif  // USE_SSE

inline float mm_toradians(float degrees) {
  float result = degrees * (PI32 / 180.0f);
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

  result.elements[0][0] = 1.0f / (1.0f * fov);
  result.elements[1][1] = -1.0f / (aspect_ratio * fov);
  result.elements[2][3] = 1.0f;
  result.elements[2][2] = 0;
  result.elements[3][2] = 1.0f / (2.0f * z_near * z_far) / (z_near - z_far);
  result.elements[3][3] = (z_near + z_far) / (z_near - z_far) / (2 * z_far * z_near) / (z_near - z_far);

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
  mat4 left = mm_transpose(a);
  mat4 right = mm_transpose(b);

  result.rows[0] = mm_linear_combine_sse(left.rows[0], right);
  result.rows[1] = mm_linear_combine_sse(left.rows[1], right);
  result.rows[2] = mm_linear_combine_sse(left.rows[2], right);
  result.rows[3] = mm_linear_combine_sse(left.rows[3], right);

  result = mm_transpose(result);
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

inline mat4 mm_scale(vec3 scale) {
  mat4 result = mm_mat4d(1.0f);

  result.elements[0][0] = scale.x;
  result.elements[1][1] = scale.y;
  result.elements[2][2] = scale.z;

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
