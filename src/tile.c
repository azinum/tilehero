// tile.c

#include "common.h"
#include "renderer.h"
#include "entity.h"
#include "camera.h"
#include "game.h"
#include "tile.h"

void tilemap_init(struct Entity* tile_map, u32 count) {
  for (u32 i = 0; i < count; i++) {
    struct Entity* tile = &tile_map[i];
    tile->tile_type = rand() % 2;
  }
}

void tilemap_render(struct Entity* tile_map, u32 count_x, u32 count_y) {
  renderer_set_tint(1, 1, 1, 1);
  for (u32 x = 0; x < count_x; x++) {
    for (u32 y = 0; y < count_y; y++) {
      struct Entity* tile = &tile_map[x + (y * count_x)];
      render_rect((x * TILE_SIZE) - camera.x, (y * TILE_SIZE) - camera.y, -0.1f, TILE_SIZE, TILE_SIZE, 0.1f, 0.2f, 0.9f, 0.3f, 0, 1.0f / TILE_SIZE);

      if (tile->tile_type != TILE_NONE) {
        render_texture_region(textures[TEXTURE_SPRITES], (x * TILE_SIZE) - camera.x, (y * TILE_SIZE) - camera.y, -0.1f, TILE_SIZE, TILE_SIZE, 0, 5 * 8, 0, 8, 8);
      }
    }
  }
}
