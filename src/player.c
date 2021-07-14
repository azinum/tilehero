// player.c

#include "game.h"
#include "entity.h"
#include "window.h"
#include "renderer_common.h"
#include "renderer.h"
#include "editor.h"
#include "player.h"

struct Player player = {
  .stunned = 0,
};

struct {
  i8 x_dir;
  i8 y_dir;
} input;

#define STUNNED_INTERVAL 0.2f
#define INTERVAL (0.01f)

void player_controller() {
  Game_state* game = &game_state;

  if (key_pressed[GLFW_KEY_LEFT] || gamepad_button_pressed[14]) {
    input.x_dir = -1;
    input.y_dir = 0;
  }
  else if (key_pressed[GLFW_KEY_RIGHT] || gamepad_button_pressed[12]) {
    input.x_dir = 1;
    input.y_dir = 0;
  }
  else if (key_pressed[GLFW_KEY_UP] || gamepad_button_pressed[11]) {
    input.y_dir = -1;
    input.x_dir = 0;
  }
  else if (key_pressed[GLFW_KEY_DOWN] || gamepad_button_pressed[13]) {
    input.y_dir = 1;
    input.x_dir = 0;
  }
  else if (key_pressed[GLFW_KEY_SPACE]) {
    input.x_dir = 0;
    input.y_dir = 0;
  }
  else if (key_pressed[GLFW_KEY_C]) {
  }
  else {
    return;
  }

  if (game->time >= move_time) {
    game->should_move = 1;
    if (player.stunned) {
      player.stunned--;
      input.x_dir = 0;
      input.y_dir = 0;
    }
    else {
      move_time = game->time + INTERVAL;
      // TODO(lucas): Add walk sound
      // audio_play_once(SOUND_0F, SFX_VOLUME / 2);
    }
  }
}

void player_update(Entity* e) {
  if (e->state == STATE_DEAD)
    return;

  e->x_dir = input.x_dir;
  e->y_dir = input.y_dir;

  if (player.stunned != 0) {
    e->x_dir = 0;
    e->y_dir = 0;
    return;
  }
  // TEMP
  else if (key_pressed[GLFW_KEY_C]) {
    Entity* item = editor_place_entity(ENTITY_HAND, e->x_tile, e->y_tile);
    if (item) {
      item->x_dir = e->x_dir;
      item->y_dir = e->y_dir;
      e->x_dir = 0;
      e->y_dir = 0;
      entity_hurt(e, 2);
    }
  }
}

