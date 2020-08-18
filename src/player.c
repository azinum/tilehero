// player.c

#include "game.h"
#include "entity.h"
#include "window.h"
#include "renderer_common.h"
#include "player.h"

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
}

