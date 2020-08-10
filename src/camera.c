// camera.c

#include "common.h"
#include "window.h"
#include "renderer_common.h"
#include "camera.h"

void camera_init(float x, float y) {
  camera.x = x;
  camera.y = y;
  camera.z = 0;
}

void camera_update() {
  if (key_pressed[GLFW_KEY_A]) {
    camera.x -= 5.0f;
  }
  if (key_pressed[GLFW_KEY_D]) {
    camera.x += 5.0f;
  }
  if (key_pressed[GLFW_KEY_W]) {
    camera.y -= 5.0f;
  }
  if (key_pressed[GLFW_KEY_S]) {
    camera.y += 5.0f;
  }
}
