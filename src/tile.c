// tile.c

#include "game.h"
#include "renderer.h"
#include "renderer_common.h"
#include "entity.h"
#include "camera.h"
#include "tile.h"

// static struct Instanced_list l = {0};

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

// #define MAX_INSTANCES 1024

void tilemap_init(struct Tile_map* tile_map, i32 x_count, i32 y_count) {
  tilemap_init_tile(tile_map, x_count, y_count, (Tile) {.tile_type = TILE_VOID, .walkable = 0, .background_tile = 0});
// instanced_list_init(&l, 4, MAX_INSTANCES, &spritesheets[SHEET_TILES].texture);
}

void tilemap_init_tile(struct Tile_map* tile_map, i32 x_count, i32 y_count, Tile tile) {
  tile_map->x_count = x_count;
  tile_map->y_count = y_count;
  for (i32 y = 0; y < y_count; y++) {
    for (i32 x = 0; x < x_count; x++) {
      Tile* current_tile = tilemap_get_tile(tile_map, x, y);
      assert(current_tile != NULL);
      *current_tile = tile;
    }
  }
}

void tilemap_render(struct Tile_map* tile_map, World_position world_position) {
#if 0
  u32 index = 0;
  struct Spritesheet sheet = spritesheets[SHEET_TILES];
  for (i32 y = 0; y < tile_map->y_count; y++) {
    for (i32 x = 0; x < tile_map->y_count; x++) {
      Tile* tile = tilemap_get_tile(tile_map, x, y);
      if (tile->tile_type == TILE_VOID) {
        index += 4;
        continue;
      }
      i32 x_offset = SHEET_GET_X_OFFSET(sheet, tile->tile_type);
      i32 y_offset = SHEET_GET_Y_OFFSET(sheet, tile->tile_type);
      l.instance_data[index++] = (x * TILE_SIZE) - camera.x;
      l.instance_data[index++] = (y * TILE_SIZE) - camera.y;
      l.instance_data[index++] = x_offset;
      l.instance_data[index++] = y_offset;
    }
  }
  l.instance_data_count = index / 4;
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, sheet.texture.id);

  glUseProgram(tile_shader);

  glUniform2f(glGetUniformLocation(tile_shader, "texture_size"), sheet.texture.w, sheet.texture.h);
  glUniform2f(glGetUniformLocation(tile_shader, "tile_size"), sheet.w, sheet.h);
  translate2d(model, 8, 8);
  glUniformMatrix4fv(glGetUniformLocation(tile_shader, "model"), 1, GL_FALSE, (float*)&model);
  render_instanced_list(&l);
#else
  i32 x_position = (world_position.x * (TILE_SIZE * TILE_COUNT_X));
  i32 y_position = (world_position.y * (TILE_SIZE * TILE_COUNT_Y));
  render_rect(x_position - camera.x, y_position - camera.y, -0.9f, TILE_SIZE * TILE_COUNT_X, TILE_SIZE * TILE_COUNT_Y, 0.35f, 0.1f, 0.35f, 1, 0, 1.0f / (TILE_SIZE * TILE_COUNT_X));

  for (i32 y = 0; y < tile_map->y_count; y++) {
    for (i32 x = 0; x < tile_map->x_count; x++) {
      struct Spritesheet sheet = spritesheets[SHEET_TILES];
      Tile* tile = &tile_map->map[x + (y * tile_map->x_count)];
      if (tile->tile_type != TILE_VOID) {
        i32 x_offset = SHEET_GET_X_OFFSET(sheet, tile->tile_type);
        i32 y_offset = SHEET_GET_Y_OFFSET(sheet, tile->tile_type);
        render_texture_region(
          sheet.texture,
          x_position + (x * TILE_SIZE) - camera.x,
          y_position + (y * TILE_SIZE) - camera.y,
          -0.1f,
          TILE_SIZE, TILE_SIZE,
          0,
          x_offset, y_offset, sheet.w, sheet.h
        );
      }
      if (tile->background_tile != 0) {
        i32 x_offset = SHEET_GET_X_OFFSET(sheet, tile->background_tile);
        i32 y_offset = SHEET_GET_Y_OFFSET(sheet, tile->background_tile);
        render_texture_region(
          sheet.texture,
          x_position + (x * TILE_SIZE) - camera.x,
          y_position + (y * TILE_SIZE) - camera.y,
          -0.1f,
          TILE_SIZE, TILE_SIZE,
          0,
          x_offset, y_offset, sheet.w, sheet.h
        );
      }
    }
  }
#endif
}

void tilemap_render_tile_highlight(struct Tile_map* tile_map, i32 x_tile, i32 y_tile) {
  World_position world_position = game_state.world.current_origin;
  i32 x_position = (world_position.x * TILE_COUNT_X * TILE_SIZE) + (x_tile * TILE_SIZE);
  i32 y_position = (world_position.y * TILE_COUNT_Y * TILE_SIZE) + (y_tile * TILE_SIZE);

  Tile* tile = tilemap_get_tile(tile_map, x_tile, y_tile);
  if (tile) {
    render_rect(x_position - camera.x, y_position - camera.y, -0.1f, TILE_SIZE, TILE_SIZE, 1, 1, 1, 1, 0, 1.0f / TILE_SIZE);
  }
}
