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
  for (i32 i = 0; i < (x_count * y_count); i++) {
    Tile* tile = &tile_map->map[i];
    tile->tile_type = rand() % 20;
  }
}

void tilemap_render(struct Tile_map* tile_map) {
  renderer_set_tint(1, 1, 1, 1);
  for (i32 x = 0; x < tile_map->x_count; x++) {
    for (i32 y = 0; y < tile_map->y_count; y++) {
      Tile* tile = &tile_map->map[x + (y * tile_map->x_count)];
      render_rect((x * TILE_SIZE) - camera.x, (y * TILE_SIZE) - camera.y, -0.1f, TILE_SIZE, TILE_SIZE, 0.1f, 0.2f, 0.9f, 0.3f, 0, 1.0f / TILE_SIZE);

      if (tile->tile_type == TILE_BRICK) {
        render_texture_region(textures[TEXTURE_SPRITES], (x * TILE_SIZE) - camera.x, (y * TILE_SIZE) - camera.y, -0.1f, TILE_SIZE, TILE_SIZE, 0, 5 * 8, 0, 8, 8);
      }
    }
  }
}