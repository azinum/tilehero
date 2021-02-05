// camera.h

#ifndef _CAMERA_H
#define _CAMERA_H

struct {
  float x;
  float y;
  float x_target;
  float y_target;
  float zoom;
  struct Entity* target;
  u8 has_target;
} camera;

void camera_init(i32 x, i32 y);

void camera_update();

#endif
