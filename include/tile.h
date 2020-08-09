// tile.h

#ifndef _TILE_H
#define _TILE_H

#define TILE_COUNT_X (24)
#define TILE_COUNT_Y (12)
#define TILE_SIZE (32)

enum Tile_type {
  TILE_NONE,
  TILE_BRICK,
  MAX_TILE,
};

void tilemap_init(struct Entity* tile_map, u32 count);

void tilemap_render(struct Entity* tile_map, u32 count_x, u32 count_y);

#endif
