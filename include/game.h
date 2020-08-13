// game.h

#ifndef _GAME_H
#define _GAME_H

#include "common.h"
#include "game_common.h"
#include "config.h"
#include "camera.h"
#include "entity.h"
#include "resource.h"
#include "tile.h"

#define MAX_ENTITY (1024)

enum Game_mode {
  MODE_GAME,
  MODE_PAUSE,
};

typedef struct Game_state {
  struct Tile_map tile_map;
  struct Entity entities[MAX_ENTITY];
  i32 entity_count;
  float time;
  float delta_time;
  float move_timer;
  u8 is_running;
  u8 mode;
} Game_state;

extern struct Game_state game_state;

void game_entity_remove(struct Entity* e);

Entity* game_add_entity(float x, float y, float w, float h);

Entity* game_add_empty_entity();

Entity* game_add_living_entity(i32 x_tile, i32 y_tile, float w, float h, i8 x_dir, i8 y_dir, i16 health, i16 max_health, i16 attack);

i32 game_execute(i32 window_width, i32 window_height, u8 fullscreen);

void game_restart();

#endif
