  // tile.h

#ifndef _TILE_H
#define _TILE_H

#include "entity.h"

#define TILE_COUNT_X (32)
#define TILE_COUNT_Y (18)
#define TILE_SIZE (42)
#define PIXEL_TO_TILE_POS(PX) (PX >= 0 ? (i32)((PX) / TILE_SIZE) : -1)

enum Tile_type {
  TILE_NONE,
  TILE_DEFAULT,
  TILE_BRICK_1,
  TILE_BRICK_2,
  TILE_DUNGEON,
  TILE_SWAPPER,
  MAX_TILE,
};

struct Tile_info {
  i32 tile_type;
};

#define USE_ENTITY_AS_TILE 0

#if USE_ENTITY_AS_TILE
typedef struct Entity Tile;
#else
typedef struct Tile_info Tile;
#endif

typedef struct Tile_map {
  i32 x_count;
  i32 y_count;
  Tile map[TILE_COUNT_X * TILE_COUNT_Y];
} Tile_map;

Tile* tilemap_get_tile(struct Tile_map* tile_map, i32 x, i32 y);

void tilemap_init(struct Tile_map* tile_map, i32 x_count, i32 y_count);

i32 tilemap_store(struct Tile_map* tile_map, const char* path);

i32 tilemap_load(struct Tile_map* tile_map, const char* path);

void tilemap_render(struct Tile_map* tile_map);

void tilemap_render_tile_highlight(struct Tile_map* tile_map, i32 x_tile, i32 y_tile);

#endif
