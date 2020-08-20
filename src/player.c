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

#define INTERVAL 0.01f
#define STUNNED_INTERVAL 0.2f

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

  u8 move = 0;
  if (player.stunned != 0) {
    move = 1;
  }
  else if (key_pressed[GLFW_KEY_LEFT]) {
    e->x_dir = -1;
    e->y_dir = 0;
    move = 1;
  }
  else if (key_pressed[GLFW_KEY_RIGHT]) {
    e->x_dir = 1;
    e->y_dir = 0;
    move = 1;
  }
  else if (key_pressed[GLFW_KEY_UP]) {
    e->y_dir = -1;
    e->x_dir = 0;
    move = 1;
  }
  else if (key_pressed[GLFW_KEY_DOWN]) {
    e->y_dir = 1;
    e->x_dir = 0;
    move = 1;
  }
  else if (key_pressed[GLFW_KEY_SPACE]) {
    e->x_dir = 0;
    e->y_dir = 0;
    move = 1;
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

    move = 0;
  }

  if (!move)
    return;

  // TODO(lucas): We would probably want to be able to stun any entity, not just the player.
  if (game_state.time >= move_time) {
    game_state.should_move = 1;
    if (player.stunned) {
      player.stunned--;
      move_time = game_state.time + STUNNED_INTERVAL;
    }
    else {
      move_time = game_state.time + INTERVAL;
    }
  }
}

