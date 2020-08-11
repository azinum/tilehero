// game_common.h

#ifndef _GAME_COMMON_H
#define _GAME_COMMON_H

inline float lerp(float v0, float v1, float t) {
  return (1 - t) * v0 + t * v1;
}

#endif
