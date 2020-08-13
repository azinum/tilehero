// tile.c

#include "common.h"
#include "renderer.h"
#include "entity.h"
#include "camera.h"
#include "game.h"
#include "tile.h"

Tile* tilemap_get_tile(struct Tile_map* tile_map, i32 x, i32 y) {
  Tile* tile = NULL;

  if (x < 0 || y < 0 || x >= tile_map->x_count || y >= tile_map->y_count) {
    return NULL;
  }

  i32 tile_index = x + (y * tile_map->x_count);
  if (tile_index < 0 || tile_index >= (tile_map->x_count * tile_map->y_count)) {
    return NULL;
  }
  tile = &tile_map->map[tile_index];
  return tile;
}

void tilemap_init(struct Tile_map* tile_map, i32 x_count, i32 y_count) {
  tile_map->x_count = x_count;
  tile_map->y_count = y_count;
  for (i32 y = 0; y < y_count; y++) {
    for (i32 x = 0; x < x_count; x++) {
      Tile* tile = tilemap_get_tile(tile_map, x, y);
      assert(tile != NULL);
      if (x == 0 || y == 0 || x == (x_count - 1) || y == (y_count - 1)) {
        tile->tile_type = TILE_BRICK_1;
      }
      else {
        tile->tile_type = TILE_DEFAULT;
      }
    }
  }
}

void tilemap_render(struct Tile_map* tile_map) {
  renderer_set_tint(1, 1, 1, 1);
  for (i32 x = 0; x < tile_map->x_count; x++) {
    for (i32 y = 0; y < tile_map->y_count; y++) {
      Tile* tile = &tile_map->map[x + (y * tile_map->x_count)];
      if (tile->tile_type != TILE_NONE) {
        render_rect((x * TILE_SIZE) - camera.x, (y * TILE_SIZE) - camera.y, -0.1f, TILE_SIZE, TILE_SIZE, 0, 0, 0, 1, 0, 1.0f / TILE_SIZE);
        render_texture_region(textures[TEXTURE_SPRITES], (x * TILE_SIZE) - camera.x, (y * TILE_SIZE) - camera.y, -0.1f, TILE_SIZE, TILE_SIZE, 0, (tile->tile_type + 4) * 8, 0, 8, 8);
      }
    }
  }
}

void tilemap_render_tile_highlight(struct Tile_map* tile_map, i32 x_tile, i32 y_tile) {
  Tile* tile = tilemap_get_tile(tile_map, x_tile, y_tile);
  if (tile) {
    render_rect((x_tile * TILE_SIZE) - camera.x, (y_tile * TILE_SIZE) - camera.y, -0.1f, TILE_SIZE, TILE_SIZE, 1, 1, 1, 1, 0, 1.0f / TILE_SIZE);
  }
}
