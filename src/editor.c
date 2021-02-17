// editor.c

#include "game.h"
#include "renderer.h"
#include "renderer_common.h"
#include "window.h"
#include "level.h"
#include "editor.h"

#define UI_TEXT_BUFF_SIZE (256)
char ui_text[UI_TEXT_BUFF_SIZE] = {0};

struct {
  u32 tile_type;
  u32 entity_type;
  struct Entity copy;
  i32 has_copy;
  struct Entity* target;
  i32 has_target;
} editor = {
  .tile_type = 0,
  .entity_type = 0,
  .has_copy = 0,
  .target = NULL,
  .has_target = 0,
};

Tile placable_tiles[] = {
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
  {TILE_PORTAL, 1, 0},
  {TILE_WATER, 0, 0},
  {TILE_FLOOR_2, 1, 0},
};

static const char* placable_tile_names[] = {
  "Void",
  "Default",
  "Floor",
  "Purple Brick",
  "Gray Brick",
  "Dungeon",
  "Swapper",
  "Grass",
  "Pine Tree",
  "Portal",
  "Water",
  "Floor 2",
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

static void add_random_health(Entity* e, const Arg* arg);
static void add_random_attack(Entity* e, const Arg* arg);

struct Entity_type_def placable_entities[MAX_PLACABLE_ENTITY] = {
  {0, 0, ENTITY_FLAG_DRAW_HEALTH | ENTITY_FLAG_MOVABLE | ENTITY_FLAG_PLAYER, ENTITY_TYPE_PLAYER, SPRITE_BOY_WITH_HELM, 5, 5, 1, NULL, {}},
  {0, 1, ENTITY_FLAG_DRAW_HEALTH | ENTITY_FLAG_MOVABLE, 0, SPRITE_RED_MONSTER, 3, 3, 1, add_random_health, {.i = 1}},
  {1, 0, ENTITY_FLAG_DRAW_HEALTH | ENTITY_FLAG_MOVABLE, 0, SPRITE_MAD_SCIENTIST, 5, 5, 1, add_random_attack, {.i = 1}},
  {1, 0, ENTITY_FLAG_FRIENDLY    | ENTITY_FLAG_MOVABLE, 0, SPRITE_WIZARD, 2, 2, 1, NULL, {}},
  {1, 0, ENTITY_FLAG_DRAW_HEALTH | ENTITY_FLAG_MOVABLE | ENTITY_FLAG_FLY, 0, SPRITE_VOID_WALKER, 36, 36, 3, add_random_attack, {.i = 5}},
  {0, 0, ENTITY_FLAG_MOVABLE     | ENTITY_FLAG_FLY, ENTITY_TYPE_FLAG, SPRITE_FLAG, 2, 2, 0, NULL, {}},
  {0, 0, ENTITY_FLAG_FRIENDLY, ENTITY_TYPE_SILVER_DOOR, SPRITE_SILVER_DOOR, 10, 10, 0, NULL, {}},
  {0, 0, ENTITY_FLAG_FRIENDLY    | ENTITY_FLAG_MOVABLE | ENTITY_FLAG_PUSHABLE | ENTITY_FLAG_FLY, ENTITY_TYPE_SILVER_KEY, SPRITE_SILVER_KEY, 1, 1, 0, NULL, {}},
  {0, 0, ENTITY_FLAG_MOVABLE | ENTITY_FLAG_PUSHABLE, ENTITY_TYPE_PUSHER, SPRITE_PUSHER, 10, 10, 0, NULL, {}},
  {1, 0, ENTITY_FLAG_DRAW_HEALTH | ENTITY_FLAG_MOVABLE | ENTITY_FLAG_FLY, ENTITY_TYPE_NPC, SPRITE_HAND, 8, 8, 1, NULL, {}},
};

static const char* placable_entity_names[MAX_PLACABLE_ENTITY] = {
  "Player",
  "Red Monster",
  "Mad Scientist",
  "Wise Wizard",
  "Void Walker",
  "Red Flag",
  "Silver Door",
  "Silver Key",
  "Pusher",
  "Hand",
};

void add_random_health(Entity* e, const Arg* arg) {
  e->max_health = e->health = e->health + rand() % (arg->i);
}

void add_random_attack(Entity* e, const Arg* arg) {
  e->attack += rand() % arg->i;
}

// NOTE(lucas): Make sure this function is split up into: ui code (should be first), rendering only code and other (keyboard input e.t.c.).
void editor_update(struct Game_state* game) {
  ui_focus(UI_EDITOR);
  struct UI_element* e = NULL;
  World* world = &game->world;
  Level* level = &world->level;

  if (ui_do_button(UI_ID, VW(2), window.height - (16 * 4), 16 * 12, 16 * 3, "Reload [r]", 24, &e) || key_pressed[GLFW_KEY_R]) {
    game_restart();
    game_send_message("Loaded level %i", level->index);
  }
  UI_INIT(e,
    e->background_color = COLOR_WARN;
    e->border = 0;
  );

  if (ui_do_button(UI_ID, VW(2), window.height - (16 * 8), 16 * 12, 16 * 3, "Save [m]", 24, &e) || key_pressed[GLFW_KEY_M]) {
    level_store(world, level->index);
    game_send_message("Level %i saved!", level->index);
  }
  UI_INIT(e,
    e->background_color = COLOR_ACCEPT;
    e->border = 0;
  );

  if (ui_do_button(UI_ID, VW(2), window.height - (16 * 14), 16 * 11, 16 * 2, "Prev level [b]", 15, &e) || key_pressed[GLFW_KEY_B]) {
    if (game_load_level(level->index - 1) == NO_ERR) {
      game_send_message("Loaded level %i", level->index);
    }
  }
  UI_INIT(e,
    e->background_color = COLOR_OK;
    e->border = 0;
  );

  if (ui_do_button(UI_ID, VW(2), window.height - (16 * 11), 16 * 11, 16 * 2, "Next level [n]", 15, &e) || key_pressed[GLFW_KEY_N]) {
    game_load_level(level->index + 1);
    game_send_message("Loaded level %i", level->index);
  }
  UI_INIT(e,
    e->background_color = COLOR_OK;
    e->border = 0;
  );
{
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
    "level: %i\n"
    ,
    (i32)camera.x, (i32)camera.y,
    window.width, window.height,
    game_state.time,
    (i32)(100 * game_state.time_scale),
    (i32)(1.0f / game_state.delta_time),
    world->entity_count, MAX_ENTITY,
    audio_engine.master_volume,
    audio_engine.sound_count, MAX_ACTIVE_SOUNDS,
    level->index
  );
  ui_do_text(UI_ID, window.width - (VW(2) + (16 * 14)), window.height - (16 * 15), 16 * 15, 16 * 14, ui_text, 12, NULL);
}

  i32 x_tile = PIXEL_TO_TILE_POS(window.mouse_x + camera.x);
  i32 y_tile = PIXEL_TO_TILE_POS(window.mouse_y + camera.y);
  if (!ui.is_interacting) {
    tilemap_render_tile_highlight(&level->tile_map, x_tile, y_tile);
  }

  if (left_mouse_down && !ui.is_interacting) {
    Tile* tile = tilemap_get_tile(&level->tile_map, x_tile, y_tile);
    if (tile) {
      Tile new_tile = placable_tiles[editor.tile_type];
      *tile = new_tile;
      if (left_mouse_pressed) {
        audio_play_once(SOUND_0F, UI_VOLUME);
      }
    }
  }
  if (right_mouse_pressed && !ui.is_interacting) {
    if (editor.has_target && editor.target) {
      editor.has_target = 0;
      editor.target = NULL;
    }
  }

  if (editor.has_target && editor.target) {
    struct Entity* e = editor.target;
    if (key_pressed[GLFW_KEY_LEFT]) {
      e->x_tile -= 1;
    }
    if (key_pressed[GLFW_KEY_RIGHT]) {
      e->x_tile += 1;
    }
    if (key_pressed[GLFW_KEY_UP]) {
      e->y_tile -= 1;
    }
    if (key_pressed[GLFW_KEY_DOWN]) {
      e->y_tile += 1;
    }
    e->x = TILE_SIZE * e->x_tile;
    e->y = TILE_SIZE * e->y_tile;
  }

  for (u32 i = 0; i < world->entity_count; i++) {
    Entity* e = &world->entities[i];
    if (mouse_over(window.mouse_x + camera.x, window.mouse_y + camera.y, e->x, e->y, e->w, e->h)) {
      entity_render_highlight(e);
      if (key_pressed[GLFW_KEY_F]) {
        camera.target = e;
        camera.has_target = 1;
      }
      if (right_mouse_pressed) {
        editor.target = e;
        editor.has_target = 1;
      }
      if (key_pressed[GLFW_KEY_X]) {
        editor.copy = *e;
        editor.has_copy = 1;
        game_send_message("Cut entity (%i)", editor.copy.id);
        if (editor.target == e && editor.has_target) {
          editor.target = NULL;
          editor.has_target = 0;
        }
        game_entity_remove(e);
        audio_play_once(SOUND_HIT, UI_VOLUME);
      }
      if (key_pressed[GLFW_KEY_C]) {
        editor.copy = *e;
        editor.has_copy = 1;
        game_send_message("Copied entity (%i)", editor.copy.id);
      }
    }
    if (editor.target == e && editor.has_target) {
      entity_render_highlight_color(e, 0.4f, 0.9f, 1.0f, 0.7f + 0.3f * sin(game->total_time * 10.0f));
    }
  }
{
  char ui_text[UI_TEXT_BUFF_SIZE] = {0};
  i32 x = 10;
  i32 y = 10;
  i32 w = TILE_SIZE;
  i32 h = w;
  Tile tile = placable_tiles[editor.tile_type];
  struct Spritesheet sheet = spritesheets[SHEET_TILES];
  i32 x_offset = SHEET_GET_X_OFFSET(sheet, tile.type);
  i32 y_offset = SHEET_GET_Y_OFFSET(sheet, tile.type);
  render_rect(x, y, 0.9f, w, h, 0.8f, 0.1f, 0.1f, 1, 0, 2.0f / TILE_SIZE);
  render_texture_region(sheet.texture, x, y, 0.9f, w, h, 0, x_offset, y_offset, sheet.w, sheet.h);
  snprintf(ui_text, UI_TEXT_BUFF_SIZE,
    "%s\n"
    ,
    placable_tile_names[editor.tile_type]
  );
  render_simple_text(textures[TEXTURE_FONT], x + w + 5, y, 0.9f, 500, 600, 12, 0.7f, 0.7f, 5.0f, ui_text, UI_TEXT_BUFF_SIZE);
}

{
  i32 w = TILE_SIZE >> 1;
  i32 h = w;
  i32 x = 10 + 10 + TILE_SIZE;
  i32 y = 10 + TILE_SIZE - h;
  Tile tile = placable_tiles[editor.tile_type];
  struct Spritesheet sheet = spritesheets[SHEET_TILES];
  i32 x_offset = SHEET_GET_X_OFFSET(sheet, tile.background_tile);
  i32 y_offset = SHEET_GET_Y_OFFSET(sheet, tile.background_tile);
  render_texture_region(sheet.texture, x, y, 0.9f, w, h, 0, x_offset, y_offset, sheet.w, sheet.h);
}
{
  char ui_text[UI_TEXT_BUFF_SIZE] = {0};
  i32 x = 10;
  i32 y = 10 + 10 + (1 * TILE_SIZE);
  i32 w = TILE_SIZE;
  i32 h = w;
  struct Spritesheet sheet = spritesheets[SHEET_ENTITIES];
  struct Entity_type_def entity = placable_entities[editor.entity_type];
  i32 x_offset = SHEET_GET_X_OFFSET(sheet, entity.sprite_id);
  i32 y_offset = SHEET_GET_Y_OFFSET(sheet, entity.sprite_id);
  render_rect(x, y, 0.9f, w, h, 0.25f, 0.80f, 0.18f, 1, 0, 2.0f / TILE_SIZE);
  render_texture_region(sheet.texture, x, y, 0.9f, w, h, 0, x_offset, y_offset, sheet.w, sheet.h);
  snprintf(ui_text, UI_TEXT_BUFF_SIZE,
    "%s\n"
    "health: %i/%i\n"
    "attack: %i\n"
    "type: 0x%x\n"
    "e_flags: 0x%x\n"
    ,
    placable_entity_names[editor.entity_type],
    entity.health, entity.max_health,
    entity.attack,
    entity.type,
    entity.e_flags
  );
  render_simple_text(textures[TEXTURE_FONT], x + w + 5, y, 0.9f, 500, 600, 12, 0.7f, 0.7f, 5.0f, ui_text, UI_TEXT_BUFF_SIZE);
}
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
    world->entity_count = 0;
    tilemap_init(&level->tile_map, TILE_COUNT_X, TILE_COUNT_Y);
  }
  if (key_pressed[GLFW_KEY_8]) {
    tilemap_init_tile(&level->tile_map, TILE_COUNT_X, TILE_COUNT_Y, placable_tiles[editor.tile_type]);
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
    editor_place_entity(editor.entity_type, x_tile, y_tile);
    audio_play_once(SOUND_0F, UI_VOLUME);
  }

  if (key_pressed[GLFW_KEY_V]) {
    if (editor.has_copy) {
      Entity* e = game_add_empty_entity();
      if (e) {
        *e = editor.copy;
        i32 x_tile = PIXEL_TO_TILE_POS(window.mouse_x + camera.x);
        i32 y_tile = PIXEL_TO_TILE_POS(window.mouse_y + camera.y);
        entity_init_tilepos(e, x_tile, y_tile);
        game_send_message("Pasted entity (%i)", e->id);
      }
      else {
        game_send_message("Failed to paste entity");
      }
    }
    else {
      game_send_message("Failed to paste entity");
    }
  }

  tilemap_render_boundary();
}

struct Entity* editor_place_entity(i16 placable_id, i32 x_tile, i32 y_tile) {
  if (placable_id >= MAX_PLACABLE_ENTITY) {
    return NULL;
  }
  struct Entity_type_def entity = placable_entities[placable_id];
  Entity* e = game_add_living_entity(x_tile, y_tile, TILE_SIZE, TILE_SIZE, entity.x_dir, entity.y_dir, entity.health, entity.max_health, entity.attack);
  if (e) {
    e->placable_id = placable_id;
    e->sprite_id = entity.sprite_id;
    e->type = entity.type;
    e->e_flags = entity.e_flags;
    if (entity.func) {
      entity.func(e, &entity.arg);
    }
    return e;
  }
  return NULL;
}

struct Entity* editor_place_entity_from_def(struct Entity_def* def) {
  assert(def);
  if (def->placable_id < 0 || def->placable_id >= MAX_PLACABLE_ENTITY) {
    return NULL;
  }
  struct Entity_type_def entity = placable_entities[def->placable_id];
  Entity* e = game_add_living_entity(def->x_tile, def->y_tile, TILE_SIZE, TILE_SIZE, entity.x_dir, entity.y_dir, entity.health, entity.max_health, entity.attack);
  if (e) {
    e->id = def->id;
    e->target_id = def->target_id;
    e->placable_id = def->placable_id;

    e->e_flags = entity.e_flags;
    e->type = entity.type;
    e->sprite_id = entity.sprite_id;

    if (entity.func) {
      entity.func(e, &entity.arg);
    }
    return e;
  }
  return NULL;
}
