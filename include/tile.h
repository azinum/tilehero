  // tile.h

#ifndef _TILE_H
#define _TILE_H

#include "entity.h"

#define TILE_COUNT_X (32)
#define TILE_COUNT_Y (32)
#define TILE_SIZE (48)
#define PIXEL_TO_TILE_POS(PX) (PX >= 0 ? (i32)((PX) / TILE_SIZE) : -1)

enum Tile_type {
  TILE_VOID,
  TILE_DEFAULT,
  TILE_FLOOR,
  TILE_BRICK_1,
  TILE_BRICK_2,
  TILE_DUNGEON,
  TILE_SWAPPER,
  TILE_GRASS,
  TILE_TREE,
  TILE_SILVER_DOOR,

  MAX_TILE,
};

typedef struct Tile {
  u8 type;
  u8 walkable;
  u8 background_tile;
} Tile;

typedef struct Tile_map {
  i32 x_count;
  i32 y_count;
  Tile map[TILE_COUNT_X * TILE_COUNT_Y];
} Tile_map;

Tile* tilemap_get_tile(struct Tile_map* tile_map, i32 x, i32 y);

void tilemap_init(struct Tile_map* tile_map, i32 x_count, i32 y_count);

void tilemap_init_tile(struct Tile_map* tile_map, i32 x_count, i32 y_count, Tile tile);

void tilemap_render(struct Tile_map* tile_map);

void tilemap_render_boundary();

void tilemap_render_tile_highlight(struct Tile_map* tile_map, i32 x_tile, i32 y_tile);

#endif
