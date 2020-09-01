// player.c

#include "game.h"
#include "entity.h"
#include "window.h"
#include "renderer_common.h"
#include "renderer.h"
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

  if (key_pressed[GLFW_KEY_LEFT]) {
    input.x_dir = -1;
    input.y_dir = 0;
  }
  else if (key_pressed[GLFW_KEY_RIGHT]) {
    input.x_dir = 1;
    input.y_dir = 0;
  }
  else if (key_pressed[GLFW_KEY_UP]) {
    input.y_dir = -1;
    input.x_dir = 0;
  }
  else if (key_pressed[GLFW_KEY_DOWN]) {
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
    }
    else {
      move_time = game->time + INTERVAL;
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
  else if (key_pressed[GLFW_KEY_C]) {
    Entity* item = game_add_empty_entity();
    entity_init_tilepos(item, e->x_tile, e->y_tile, TILE_SIZE, TILE_SIZE);
    item->x_dir = e->x_dir;
    item->y_dir = e->y_dir;
    item->e_flags = ENTITY_FLAG_FRIENDLY | ENTITY_FLAG_MOVABLE | ENTITY_FLAG_FLY;
    item->type = ENTITY_TYPE_CONSUMABLE;
    item->sprite_id = SPRITE_COOKIE;
    item->health = item->max_health = 1;
    e->x_dir = 0;
    e->y_dir = 0;
  }
}

