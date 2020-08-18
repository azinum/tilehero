// editor.c

#include "game.h"
#include "renderer.h"
#include "renderer_common.h"
#include "window.h"
#include "world.h"
#include "editor.h"

struct {
  u32 tile_type;
  u32 chunk_index;
} editor = {
  .tile_type = TILE_NONE,
};

void editor_update() {
#if USE_EDITOR
  i32 x_tile = PIXEL_TO_TILE_POS(window.mouse_x + camera.x);
  i32 y_tile = PIXEL_TO_TILE_POS(window.mouse_y + camera.y);
  if (x_tile >= 0 && x_tile < TILE_COUNT_X && y_tile >= 0 && y_tile < TILE_COUNT_Y) {
    tilemap_render_tile_highlight(&game_state.world_chunk.tile_map, x_tile, y_tile);
  }
  if (key_pressed[GLFW_KEY_T]) {
    i32 x_tile = PIXEL_TO_TILE_POS(window.mouse_x + camera.x);
    i32 y_tile = PIXEL_TO_TILE_POS(window.mouse_y + camera.y);
    if (x_tile >= 0 && x_tile < TILE_COUNT_X && y_tile >= 0 && y_tile < TILE_COUNT_Y) {
      i16 health = 5 + rand() % 10;
      i16 attack = 1 + rand() % 3;
      Entity* e = game_add_living_entity(x_tile, y_tile, TILE_SIZE, TILE_SIZE, 0, 1, health, health, attack);
      if (e) {
        e->sprite_id = SPRITE_RED_MONSTER;
        audio_play_once(SOUND_0F, 0.5f);
      }
    }
  }
  if (key_pressed[GLFW_KEY_Y]) {
    i32 x_tile = PIXEL_TO_TILE_POS(window.mouse_x + camera.x);
    i32 y_tile = PIXEL_TO_TILE_POS(window.mouse_y + camera.y);
    if (x_tile >= 0 && x_tile < TILE_COUNT_X && y_tile >= 0 && y_tile < TILE_COUNT_Y) {
      i16 health = 5 + rand() % 10;
      i16 attack = 1 + rand() % 3;
      Entity* e = game_add_living_entity(x_tile, y_tile, TILE_SIZE, TILE_SIZE, 1, 0, health, health, attack);
      if (e) {
        e->sprite_id = SPRITE_WIZARD;
        e->e_flags |= ENTITY_FLAG_FRIENDLY;
        e->e_flags ^= ENTITY_FLAG_DRAW_HEALTH;
        audio_play_once(SOUND_0F, 0.5f);
      }
    }
  }
  if (key_pressed[GLFW_KEY_U]) {
    i32 x_tile = PIXEL_TO_TILE_POS(window.mouse_x + camera.x);
    i32 y_tile = PIXEL_TO_TILE_POS(window.mouse_y + camera.y);
    if (x_tile >= 0 && x_tile < TILE_COUNT_X && y_tile >= 0 && y_tile < TILE_COUNT_Y) {
      Entity* e = game_add_living_entity(x_tile, y_tile, TILE_SIZE, TILE_SIZE, 0, 0, 1, 1, 1);
      if (e) {
        e->sprite_id = SPRITE_BOY_WITH_HELM;
        e->e_flags |= ENTITY_FLAG_FRIENDLY;
        e->e_flags ^= ENTITY_FLAG_DRAW_HEALTH;
        audio_play_once(SOUND_0F, 0.5f);
      }
    }
  }
  if (key_pressed[GLFW_KEY_E]) {
    editor.tile_type = (editor.tile_type + 1) % MAX_TILE;
  }

  if (key_pressed[GLFW_KEY_1]) {
    game_state.time_scale *= 0.9f;;
    if (game_state.time_scale <= TIME_SCALING_MIN)
      game_state.time_scale = TIME_SCALING_MIN;
  }
  if (key_pressed[GLFW_KEY_2]) {
    game_state.time_scale *= 1.1f;
    if (game_state.time_scale >= TIME_SCALING_MAX)
      game_state.time_scale = TIME_SCALING_MAX;
  }
  if (key_pressed[GLFW_KEY_3]) {
    game_state.time_scale = 1;
  }

  if (key_pressed[GLFW_KEY_9]) {
    tilemap_init(&game_state.world_chunk.tile_map, TILE_COUNT_X, TILE_COUNT_Y);
  }

  if (left_mouse_down || key_down[GLFW_KEY_R]) {
    i32 x_tile = PIXEL_TO_TILE_POS(window.mouse_x + camera.x);
    i32 y_tile = PIXEL_TO_TILE_POS(window.mouse_y + camera.y);
    if (x_tile >= 0 && x_tile < TILE_COUNT_X && y_tile >= 0 && y_tile < TILE_COUNT_Y) {
      Tile* tile = tilemap_get_tile(&game_state.world_chunk.tile_map, x_tile, y_tile);
      if (tile) {
        tile->tile_type = editor.tile_type;
        if (left_mouse_pressed) {
          audio_play_once(SOUND_0F, 0.5f);
        }
      }
    }
  }
  if (key_pressed[GLFW_KEY_N]) {
    game_state.world_chunk.chunk_index = editor.chunk_index;
    world_chunk_store(&game_state.world_chunk, WORLD_STORAGE_FILE);
  }
  if (key_pressed[GLFW_KEY_M]) {
    world_chunk_load(&game_state.world_chunk, editor.chunk_index, WORLD_STORAGE_FILE);
  }
  if (key_pressed[GLFW_KEY_V]) {
    game_state.world_chunk.chunk_index = editor.chunk_index;
    world_chunk_store(&game_state.world_chunk, WORLD_STORAGE_FILE);
    editor.chunk_index--;
    world_chunk_load(&game_state.world_chunk, editor.chunk_index, WORLD_STORAGE_FILE);
  }
  if (key_pressed[GLFW_KEY_B]) {
    game_state.world_chunk.chunk_index = editor.chunk_index;
    world_chunk_store(&game_state.world_chunk, WORLD_STORAGE_FILE);
    editor.chunk_index++;
    if (world_chunk_load(&game_state.world_chunk, editor.chunk_index, WORLD_STORAGE_FILE) < 0) {
      tilemap_init(&game_state.world_chunk.tile_map, TILE_COUNT_X, TILE_COUNT_Y);
    }
  }
#endif
}

#define UI_TEXT_BUFF_SIZE (256)
char ui_text[UI_TEXT_BUFF_SIZE] = {0};

void editor_render() {
#if USE_EDITOR
{
  i32 x = 10;
  i32 y = 10;
  i32 w = TILE_SIZE * 2;
  i32 h = w;
  struct Spritesheet sheet = spritesheets[SHEET_TILES];
  i32 x_offset = SHEET_GET_X_OFFSET(sheet, editor.tile_type);
  i32 y_offset = SHEET_GET_Y_OFFSET(sheet, editor.tile_type);
  render_rect(x, y, 0.9f, w, h, 0.8f, 0.20f, 0.25f, 1, 0, 1.0f / TILE_SIZE);
  render_texture_region(sheet.texture, x, y, 0.9f, w, h, 0, x_offset, y_offset, sheet.w, sheet.h);
}
  i32 w = 230;
  i32 h = 200;
  snprintf(
    ui_text,
    UI_TEXT_BUFF_SIZE,
    "camera x: %i, y: %i\n"
    "window size: %ix%i\n"
    "time: %.3f\n"
    "time scale: %i %%\n"
    "fps: %i\n"
    "entity count: %i/%i\n"
    "master volume: %.2f\n"
    "active sounds: %i/%i\n"
    "chunk index: %i\n"
    ,
    (i32)camera.x, (i32)camera.y,
    window.width, window.height,
    game_state.time,
    (i32)(100 * game_state.time_scale),
    (i32)(1.0f / game_state.delta_time),
    game_state.world_chunk.entity_count, MAX_ENTITY,
    audio_engine.master_volume,
    audio_engine.sound_count, MAX_ACTIVE_SOUNDS,
    editor.chunk_index);
  render_text(textures[TEXTURE_FONT],
    10, window.height - 10 - h, // x, y
    0.9f, // z
    w,   // Width
    h, // Height
    12, // Font size
    0.7f, // Font kerning
    0.7f, // Line spacing
    12.0f, // Margin
    ui_text,
    UI_TEXT_BUFF_SIZE
  );

#else
#endif

{
  i32 w = 140;
  i32 h = 35;
  i32 x = window.width - (w + 10);
  i32 y = 10;
  if (game_state.mode == MODE_PAUSE) {
    snprintf(ui_text, UI_TEXT_BUFF_SIZE, "[game paused]");
    render_text(textures[TEXTURE_FONT],
      x, y, // x, y
      0.9f, // z
      w,   // Width
      h, // Height
      12, // Font size
      0.7f, // Font kerning
      0.7f, // Line spacing
      12.0f, // Margin
      ui_text,
      UI_TEXT_BUFF_SIZE
    );
  }
}
  if (camera.has_target && camera.target != NULL) {
    snprintf(ui_text, UI_TEXT_BUFF_SIZE, "[camera locked]");
    render_text(textures[TEXTURE_FONT],
      window.width - 10 - 160, window.height - 10 - 35, // x, y
      0.9f, // z
      160,   // Width
      35, // Height
      12, // Font size
      0.7f, // Font kerning
      0.7f, // Line spacing
      12.0f, // Margin
      ui_text,
      UI_TEXT_BUFF_SIZE
    );
  }
}

