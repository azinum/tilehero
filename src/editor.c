// editor.c

#include "game.h"
#include "renderer.h"
#include "renderer_common.h"
#include "window.h"
#include "world.h"
#include "editor.h"

struct {
  u32 tile_type;
  u32 entity_type;
  u32 chunk_index;
} editor = {
  .tile_type = 0,
  .entity_type = 0,
  .chunk_index = 0,
};

static Tile placable_tiles[] = {
  // type, walkable, background
  {TILE_VOID, 0, 0},
  {TILE_DEFAULT, 1, 0},
  {TILE_FLOOR, 1, 0},
  {TILE_BRICK_1, 0, 0},
  {TILE_BRICK_2, 0, 0},
  {TILE_DUNGEON, 1, 0},
  {TILE_SWAPPER, 1, 0},
  {TILE_GRASS, 1, 0},
  {TILE_TREE, 0, TILE_GRASS},
};

typedef union Arg {
  i32 i;
  float f;
  double d;
} Arg;

typedef void (*init_func)(Entity* e, const Arg*);

struct Entity_type_def {
  i8 x_dir;
  i8 y_dir;
  i32 e_flags;
  i16 type;
  i16 sprite_id;
  i16 health;
  i16 max_health;
  i16 attack;
  init_func func;
  const Arg arg;
};

enum Placable_entity_type {
  ENTITY_PLAYER,
  ENTITY_RED_MONSTER,
  ENTITY_MAD_SCIENTIST,
  ENTITY_WIZARD,
  ENTITY_VOID_WALKER,

  MAX_PLACABLE_ENTITY,
};

static void add_random_health(Entity* e, const Arg* arg);
static void add_random_attack(Entity* e, const Arg* arg);

static struct Entity_type_def placable_entities[MAX_PLACABLE_ENTITY] = {
  {0, 0, ENTITY_FLAG_MOVABLE | ENTITY_FLAG_PLAYER, ENTITY_TYPE_PLAYER, SPRITE_BOY_WITH_HELM, 10, 10, 3, NULL, {}},
  {0, 1, ENTITY_FLAG_DRAW_HEALTH  | ENTITY_FLAG_MOVABLE, 0, SPRITE_RED_MONSTER, 5, 5, 1, add_random_health, {.i = 4}},
  {1, 0, ENTITY_FLAG_DRAW_HEALTH  | ENTITY_FLAG_MOVABLE, 0, SPRITE_MAD_SCIENTIST, 12, 12, 1, add_random_attack, {.i = 3}},
  {1, 0, ENTITY_FLAG_FRIENDLY     | ENTITY_FLAG_MOVABLE, 0, SPRITE_WIZARD, 5, 5, 1, NULL, {}},
  {1, 0, ENTITY_FLAG_DRAW_HEALTH  | ENTITY_FLAG_MOVABLE | ENTITY_FLAG_FLY, 0, SPRITE_VOID_WALKER, 35, 35, 5, add_random_attack, {.i = 5}},
};

void add_random_health(Entity* e, const Arg* arg) {
  e->max_health = e->health = e->health + rand() % (arg->i);
}

void add_random_attack(Entity* e, const Arg* arg) {
  e->attack += rand() % arg->i;
}

void editor_update() {
#if USE_EDITOR
  Level* level = &game_state.level;
  i32 x_tile = PIXEL_TO_TILE_POS(window.mouse_x + camera.x);
  i32 y_tile = PIXEL_TO_TILE_POS(window.mouse_y + camera.y);
  tilemap_render_tile_highlight(&level->tile_map, x_tile, y_tile);

  if (key_pressed[GLFW_KEY_E]) {
    editor.tile_type = (editor.tile_type + 1) % ARR_SIZE(placable_tiles);
  }
  if (key_pressed[GLFW_KEY_1]) {
    game_state.time_scale -= 0.05f;
    if (game_state.time_scale <= TIME_SCALING_MIN)
      game_state.time_scale = TIME_SCALING_MIN;
  }
  if (key_pressed[GLFW_KEY_2]) {
    game_state.time_scale += 0.05f;
    if (game_state.time_scale >= TIME_SCALING_MAX)
      game_state.time_scale = TIME_SCALING_MAX;
  }
  if (key_pressed[GLFW_KEY_3]) {
    game_state.time_scale = 1;
  }

  if (key_pressed[GLFW_KEY_9]) {
    tilemap_init(&level->tile_map, TILE_COUNT_X, TILE_COUNT_Y);
  }
  if (key_pressed[GLFW_KEY_8]) {
    tilemap_init_tile(&level->tile_map, TILE_COUNT_X, TILE_COUNT_Y, placable_tiles[editor.tile_type]);
  }

  if (left_mouse_down || key_down[GLFW_KEY_R]) {
    Tile* tile = tilemap_get_tile(&level->tile_map, x_tile, y_tile);
    if (tile) {
      Tile new_tile = placable_tiles[editor.tile_type];
      *tile = new_tile;
      if (left_mouse_pressed) {
        audio_play_once(SOUND_0F, 0.5f);
      }
    }
  }

  if (key_pressed[GLFW_KEY_N]) {
    world_level_store(level, level->index);
  }
  if (key_pressed[GLFW_KEY_M]) {
    world_level_load(level, level->index);
  }
  if (key_pressed[GLFW_KEY_V]) {
    if (level->index > 0) {
      world_level_load(level, level->index - 1);
    }
  }
  if (key_pressed[GLFW_KEY_B]) {
    world_level_load(level, level->index + 1);
  }

  if (key_pressed[GLFW_KEY_4] && editor.entity_type > 0) {
    editor.entity_type--;
  }
  if (key_pressed[GLFW_KEY_5] && editor.entity_type < (MAX_PLACABLE_ENTITY - 1)) {
    editor.entity_type++;
  }

  if (key_pressed[GLFW_KEY_Z]) {
    i32 x_tile = PIXEL_TO_TILE_POS(window.mouse_x + camera.x);
    i32 y_tile = PIXEL_TO_TILE_POS(window.mouse_y + camera.y);
    struct Entity_type_def entity = placable_entities[editor.entity_type];
    Entity* e = game_add_living_entity(x_tile, y_tile, TILE_SIZE, TILE_SIZE, entity.x_dir, entity.y_dir, entity.health, entity.max_health, entity.attack);
    if (e) {
      e->sprite_id = entity.sprite_id;
      e->type = entity.type;
      e->e_flags = entity.e_flags;
      if (entity.func) {
        entity.func(e, &entity.arg);
      }
      audio_play_once(SOUND_0F, 0.5f);
    }
  }
#endif
}

#define UI_TEXT_BUFF_SIZE (256)
char ui_text[UI_TEXT_BUFF_SIZE] = {0};

void editor_render() {
#if USE_EDITOR
  Level* level = &game_state.level;
{
  i32 x = 10;
  i32 y = 10;
  i32 w = TILE_SIZE;
  i32 h = w;
  Tile tile = placable_tiles[editor.tile_type];
  struct Spritesheet sheet = spritesheets[SHEET_TILES];
  i32 x_offset = SHEET_GET_X_OFFSET(sheet, tile.tile_type);
  i32 y_offset = SHEET_GET_Y_OFFSET(sheet, tile.tile_type);
  render_rect(x, y, 0.9f, w, h, 0.8f, 0.20f, 0.25f, 1, 0, 1.0f / TILE_SIZE);
  render_texture_region(sheet.texture, x, y, 0.9f, w, h, 0, x_offset, y_offset, sheet.w, sheet.h);
}
{
  i32 x = 10 + 10 + TILE_SIZE;
  i32 y = 10;
  i32 w = TILE_SIZE;
  i32 h = w;
  struct Spritesheet sheet = spritesheets[SHEET_ENTITIES];
  struct Entity_type_def entity = placable_entities[editor.entity_type];
  i32 x_offset = SHEET_GET_X_OFFSET(sheet, entity.sprite_id);
  i32 y_offset = SHEET_GET_Y_OFFSET(sheet, entity.sprite_id);
  render_rect(x, y, 0.9f, w, h, 0.25f, 0.80f, 0.18f, 1, 0, 1.0f / TILE_SIZE);
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
    "current level: %i\n"
    ,
    (i32)camera.x, (i32)camera.y,
    window.width, window.height,
    game_state.time,
    (i32)(100 * game_state.time_scale),
    (i32)(1.0f / game_state.delta_time),
    game_state.level.entity_count, MAX_ENTITY,
    audio_engine.master_volume,
    audio_engine.sound_count, MAX_ACTIVE_SOUNDS,
    level->index
  );
  render_simple_text(textures[TEXTURE_FONT],
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
    render_simple_text(textures[TEXTURE_FONT],
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
    render_simple_text(textures[TEXTURE_FONT],
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

