// camera.c

#include "common.h"
#include "window.h"
#include "renderer_common.h"
#include "camera.h"

void camera_init(float x, float y) {
  camera.x = x;
  camera.y = y;
}

void camera_update() {
  if (key_down[GLFW_KEY_A]) {
    camera.x -= 5.0f;
  }
  if (key_down[GLFW_KEY_D]) {
    camera.x += 5.0f;
  }
  if (key_down[GLFW_KEY_W]) {
    camera.y -= 5.0f;
  }
  if (key_down[GLFW_KEY_S]) {
    camera.y += 5.0f;
  }
}
