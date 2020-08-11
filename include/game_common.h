// game_common.h

#ifndef _GAME_COMMON_H
#define _GAME_COMMON_H

#define mouse_over(M_X, M_Y, X, Y, W, H) (M_X >= X && M_X <= X + W && M_Y >= Y && M_Y <= Y + H)

inline float lerp(float v0, float v1, float t) {
  return (1 - t) * v0 + t * v1;
}

#endif
