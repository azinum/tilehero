// player.c

#include "game.h"
#include "entity.h"
#include "window.h"
#include "renderer_common.h"
#include "player.h"

struct Player player = {
  .stunned = 0,
};

#define INTERVAL 0.1f
#define STUNNED_INTERVAL 0.3f

static float next_move_time = 0.0f;

void player_init(Entity* e, i32 x_tile, i32 y_tile, float w, float h) {
  entity_init_tilepos(e, x_tile, y_tile, w, h);
  e->e_flags |= ENTITY_FLAG_MOVABLE | ENTITY_FLAG_DRAW_HEALTH;
  e->x_dir = 0;
  e->y_dir = 0;
  e->health = e->max_health = 10;
  e->attack = 2;
  e->type = ENTITY_TYPE_PLAYER;
}

void player_update(Entity* e) {
  if (e->state == STATE_DEAD)
    return;

  if (key_pressed[GLFW_KEY_LEFT]) {
    e->x_dir = -1;
    e->y_dir = 0;
  }
  else if (key_pressed[GLFW_KEY_RIGHT]) {
    e->x_dir = 1;
    e->y_dir = 0;
  }
  else if (key_pressed[GLFW_KEY_UP]) {
    e->y_dir = -1;
    e->x_dir = 0;
  }
  else if (key_pressed[GLFW_KEY_DOWN]) {
    e->y_dir = 1;
    e->x_dir = 0;
  }
  else if (key_pressed[GLFW_KEY_SPACE]) {
    e->x_dir = 0;
    e->y_dir = 0;
  }
  else if (player.stunned) {
  }
  else {
    return;
  }
  if (game_state.time >= (next_move_time)) {
    game_state.should_move = 1;
    if (player.stunned) {
      next_move_time = game_state.time + (STUNNED_INTERVAL * (player.stunned != 0));
      player.stunned--;
    }
    else {
      next_move_time = game_state.time + INTERVAL;
    }
  }
}

