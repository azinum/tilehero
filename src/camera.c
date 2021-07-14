// camera.c

#include "common.h"
#include "game.h"
#include "window.h"
#include "renderer_common.h"
#include "player.h"
#include "math_util.h"
#include "game_common.h"
#include "camera.h"

#define CAMERA_MOVE_SPEED (600.0f)
#define CAMERA_TARGET_SPEED (10.0f)
#define CAMERA_CENTER_X (camera.x + (window.width >> 1))
#define CAMERA_CENTER_Y (camera.y + (window.height >> 1))

static v2 camera_pos = V2(0, 0);
static v2 mouse_pos = V2(0, 0);

void camera_init(i32 x, i32 y) {
  camera.x = x;
  camera.y = y;
  camera.x_target = x;
  camera.y_target = y;
  camera.zoom = 1.0f;
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

  if (joystick_present) {
    v2 joy = joysticks[0];
    camera.x_target += CAMERA_MOVE_SPEED * joy.x * game_state.delta_time;
    camera.y_target += CAMERA_MOVE_SPEED * joy.y * game_state.delta_time;
  }

  camera.has_target = (key_pressed[GLFW_KEY_Q] || gamepad_button_pressed[3] /* y */) ? (!camera.has_target) : (camera.has_target);

  if (middle_mouse_pressed) {
    camera_pos = V2(camera.x_target, camera.y_target);
    mouse_pos = V2(window.mouse_x, window.mouse_y);
  }
  else if (middle_mouse_down) {
    camera.x_target = camera_pos.x - (window.mouse_x - mouse_pos.x);
    camera.y_target = camera_pos.y - (window.mouse_y - mouse_pos.y);
  }

  if (camera.target != NULL && camera.has_target) {
    camera.x_target = camera.target->x - (window.width >> 1);
    camera.y_target = camera.target->y - (window.height >> 1);
    camera.x = lerp2(camera.x, camera.x_target, CAMERA_TARGET_SPEED * game_state.delta_time, 0.2f);
    camera.y = lerp2(camera.y, camera.y_target, CAMERA_TARGET_SPEED * game_state.delta_time, 0.2f);
  }
  else {
    camera.x = camera.x_target;
    camera.y = camera.y_target;
  }
}
