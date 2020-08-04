// camera.h

#ifndef _CAMERA_H
#define _CAMERA_H

struct {
  float x;
  float y;
  float z;
} camera;

void camera_init(float x, float y);

void camera_update();

#endif
