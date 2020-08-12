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

#define ENTITIES_MAX (256)

enum Game_mode {
  MODE_GAME,
  MODE_PAUSE,
};

typedef struct Game_state {
  struct Tile_map tile_map;
  struct Entity entities[ENTITIES_MAX];
  i32 entity_count;
  i32 tick;
  u8 is_running;
  u8 mode;
} Game_state;

extern struct Game_state game_state;

void game_entity_remove(struct Entity* e);

i32 game_execute(i32 window_width, i32 window_height, u8 fullscreen);

void game_restart();

#endif
