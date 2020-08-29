// camera.c

#include "common.h"
#include "game.h"
#include "window.h"
#include "renderer_common.h"
#include "player.h"
#include "camera.h"

#define CAMERA_MOVE_SPEED (600.0f)
#define CAMERA_TARGET_SPEED (7.0f)
#define CAMERA_CENTER_X (camera.x + (window.width >> 1))
#define CAMERA_CENTER_Y (camera.y + (window.height >> 1))

void camera_init(i32 x, i32 y) {
  camera.x = x;
  camera.y = y;
  camera.x_target = x;
  camera.y_target = y;
  camera.target = NULL;
  camera.has_target = 1;
}

void camera_update() {
  if (key_down[GLFW_KEY_A]) {
    camera.x_target -= CAMERA_MOVE_SPEED * game_state.delta_time;
  }
  if (key_down[GLFW_KEY_D]) {
    camera.x_target += CAMERA_MOVE_SPEED * game_state.delta_time;
  }
  if (key_down[GLFW_KEY_W]) {
    camera.y_target -= CAMERA_MOVE_SPEED * game_state.delta_time;
  }
  if (key_down[GLFW_KEY_S]) {
    camera.y_target += CAMERA_MOVE_SPEED * game_state.delta_time;
  }
  if (key_pressed[GLFW_KEY_Q] && camera.target != NULL) {
    camera.has_target = !camera.has_target;
  }

  if (camera.target != NULL && camera.has_target) {
    camera.x_target = camera.target->x - (window.width >> 1);
    camera.y_target = camera.target->y - (window.height >> 1);
    camera.x = lerp(camera.x, camera.x_target, CAMERA_TARGET_SPEED * game_state.delta_time);
    camera.y = lerp(camera.y, camera.y_target, CAMERA_TARGET_SPEED * game_state.delta_time);
  }
  else {
    camera.x = camera.x_target;
    camera.y = camera.y_target;
  }
}
