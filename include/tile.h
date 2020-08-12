  // tile.h

#ifndef _TILE_H
#define _TILE_H

#define TILE_COUNT_X (24)
#define TILE_COUNT_Y (12)
#define TILE_SIZE (32)

#include "entity.h"

enum Tile_type {
  TILE_NONE,
  TILE_BRICK,
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
  Tile map[TILE_COUNT_X * TILE_COUNT_Y];
  i32 x_count;
  i32 y_count;
} Tile_map;

Tile* tilemap_get_tile(struct Tile_map* tile_map, i32 x, i32 y);

void tilemap_init(struct Tile_map* tile_map, i32 x_count, i32 y_count);

void tilemap_render(struct Tile_map* tile_map);

#endif
