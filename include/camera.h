// camera.h

#ifndef _CAMERA_H
#define _CAMERA_H

struct {
  i32 x;
  i32 y;
  i32 x_target;
  i32 y_target;
  struct Entity* target;
  u8 has_target;
} camera;

void camera_init(i32 x, i32 y);

void camera_update();

#endif
