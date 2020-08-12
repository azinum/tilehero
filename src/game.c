// game.c

#include "common.h"
#include "window.h"
#include "matrix_math.h"
#include "audio.h"
#include "entity.h"
#include "renderer.h"
#include "renderer_common.h"
#include "resource.h"
#include "game.h"

Game_state game_state;

static float fade_value;
static i32 is_fading_out;

static void game_init(Game_state* game);
static void game_run();
static void dev_hud_render();
static void fade_out();

Entity* game_add_entity(float x, float y, float w, float h) {
  Entity* e = game_add_empty_entity();
  entity_init(e, x, y, w, h);
  return e;
}

Entity* game_add_empty_entity() {
  if (game_state.entity_count >= MAX_ENTITY)
    return NULL;
  return &game_state.entities[game_state.entity_count++];
}

Entity* game_add_living_entity(i32 x_tile, i32 y_tile, float w, float h, i8 x_dir, i8 y_dir, i16 health, i16 max_health, i16 attack) {
  Entity* e = game_add_empty_entity();
  if (!e)
    return NULL;
  entity_init_tilepos(e, x_tile, y_tile, w, h);
  e->x_dir = x_dir;
  e->y_dir = y_dir;
  e->state = STATE_ALIVE;
  e->e_flags |= ENTITY_FLAG_DRAW_HEALTH;
  e->health = health;
  e->max_health = max_health;
  e->attack = attack;
  return e;
}

void game_init(Game_state* game) {
  srand((u32)time(NULL));
  game->is_running = 1;
  game->entity_count = 0;
  game->tick = 0;
  game->is_running = 1;
  game->mode = MODE_GAME;

  for (u32 i = 0; i < 2; i++) {
    i16 health = 5 + rand() % 10;
    i16 attack = 1 + rand() % 3;
    game_add_living_entity(i, i, 32, 32, 0, 1, health, health, attack);
  }

  is_fading_out = 1;
  fade_value = 1.0f;
  camera_init(-(window.width / 2), -(window.height / 2));
  tilemap_init(&game_state.tile_map, TILE_COUNT_X, TILE_COUNT_Y);
  // audio_play_once_on_channel(SOUND_SONG_METAKING, 0, 0.1f);
}

void game_run() {
  game_init(&game_state);
  while (game_state.is_running && !window_process_input() && !window_should_close()) {
    window_pollevents();

    if (key_pressed[GLFW_KEY_T]) {
      i32 x_tile = (i32)((window.mouse_x + camera.x) / TILE_SIZE);
      i32 y_tile = (i32)((window.mouse_y + camera.y) / TILE_SIZE);
      if (x_tile >= 0 && x_tile < TILE_COUNT_X && y_tile >= 0 && y_tile < TILE_COUNT_Y) {
        i16 health = 5 + rand() % 10;
        i16 attack = 1 + rand() % 3;
        game_add_living_entity(x_tile, y_tile, TILE_SIZE, TILE_SIZE, 0, 1, health, health, attack);
        audio_play_once(SOUND_0F, 0.2f);
      }
    }
    if (key_pressed[GLFW_KEY_Y]) {
      i32 x_tile = (i32)((window.mouse_x + camera.x) / TILE_SIZE);
      i32 y_tile = (i32)((window.mouse_y + camera.y) / TILE_SIZE);
      if (x_tile >= 0 && x_tile < TILE_COUNT_X && y_tile >= 0 && y_tile < TILE_COUNT_Y) {
        i16 health = 5 + rand() % 10;
        i16 attack = 1 + rand() % 3;
        Entity* e = game_add_living_entity(x_tile, y_tile, TILE_SIZE, TILE_SIZE, 1, 0, health, health, attack);
        e->e_flags |= ENTITY_FLAG_FRIENDLY;
        e->e_flags ^= ENTITY_FLAG_DRAW_HEALTH;
        audio_play_once(SOUND_0F, 0.2f);
      }
    }
    if (key_pressed[GLFW_KEY_R]) {
      i32 x_tile = (i32)((window.mouse_x + camera.x) / TILE_SIZE);
      i32 y_tile = (i32)((window.mouse_y + camera.y) / TILE_SIZE);
      if (x_tile >= 0 && x_tile < TILE_COUNT_X && y_tile >= 0 && y_tile < TILE_COUNT_Y) {
        Tile* tile = tilemap_get_tile(&game_state.tile_map, x_tile, y_tile);
        if (tile) {
          if (tile->tile_type != TILE_BRICK) {
            tile->tile_type = TILE_BRICK;
          }
          else {
            tile->tile_type = 0;
          }
          audio_play_once(SOUND_0F, 0.2f);
        }
      }
    }
    if (key_pressed[GLFW_KEY_0]) {
      game_restart();
    }
    if (key_pressed[GLFW_KEY_P]) {
      if (game_state.mode == MODE_GAME)
        game_state.mode = MODE_PAUSE;
      else {
        game_state.mode = MODE_GAME;
      }
    }

    if (game_state.mode == MODE_GAME) {
      game_state.tick++;
    }
    camera_update();

    for (i32 i = 0; i < game_state.entity_count; i++) {
      Entity* e = &game_state.entities[i];
      entity_update_and_render(e);
      if (mouse_over(window.mouse_x + camera.x, window.mouse_y + camera.y, e->x, e->y, e->w, e->h)) {
        entity_render_highlight(e);
        if (key_pressed[GLFW_KEY_F]) {
          camera.target = e;
          camera.has_target = 1;
        }
        if (key_pressed[GLFW_KEY_X]) {
          game_entity_remove(e);
          audio_play_once(SOUND_HIT, 0.5f);
        }
      }
    }

    if (!(game_state.tick % MOVE_INTERVAL)) {
      entity_do_tiled_move(game_state.entities, game_state.entity_count);
    }

    tilemap_render(&game_state.tile_map);
    dev_hud_render();
    if (is_fading_out) {
      fade_out();
    }
    window_swapbuffers();
    window_clear();
  }
}

#define UI_TEXT_BUFF_SIZE (256)

void dev_hud_render() {
  char ui_text[UI_TEXT_BUFF_SIZE] = {0};

#if 1
  i32 w = 230;
  i32 h = 140;
  snprintf(
    ui_text,
    UI_TEXT_BUFF_SIZE,
    "camera x: %i, y: %i\n"
    "window size: %ix%i\n"
    "tick: %i\n"
    "entity count: %i/%i\n"
    "master volume: %.3g\n"
    "active sounds: %i/%i\n"
    ,
    (i32)camera.x, (i32)camera.y,
    window.width, window.height,
    game_state.tick,
    game_state.entity_count, MAX_ENTITY,
    audio_engine.master_volume,
    audio_engine.sound_count, MAX_ACTIVE_SOUNDS);
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
  if (game_state.mode == MODE_PAUSE) {
    snprintf(ui_text, UI_TEXT_BUFF_SIZE, "[game paused]");
    render_text(textures[TEXTURE_FONT],
      10, 10, // x, y
      0.9f, // z
      140,   // Width
      35, // Height
      12, // Font size
      0.7f, // Font kerning
      0.7f, // Line spacing
      12.0f, // Margin
      ui_text,
      UI_TEXT_BUFF_SIZE
    );
  }
#else
#endif
}

void fade_out() {
  fade_value = lerp(fade_value, 0.0f, 0.05f);
  render_filled_rect(0, 0, 1, window.width, window.height, 0, 0, 0, fade_value, 0);
  if (fade_value < 0.01f) {
    fade_value = 0;
    is_fading_out = 0;
  }
}

void game_entity_remove(Entity* e) {
  if (game_state.entity_count > 0) {
    Entity* top = &game_state.entities[--game_state.entity_count];
    if (e == camera.target) {
      camera.target = NULL;
      camera.has_target = 0;
    }
    if (top == camera.target) {
      camera.target = e;
    }
    *e = *top;
    return;
  }
  assert(0);
}

i32 game_execute(i32 window_width, i32 window_height, u8 fullscreen) {
  if (window_open(window_width, window_height, fullscreen, "Tile Hero") != 0) {
    fprintf(stderr, "Failed to open window\n");
    return -1;
  }
  resources_load();
  if (audio_engine_init(SAMPLE_RATE, FRAMES_PER_BUFFER, game_run) != 0) {
    fprintf(stderr, "Failed to initialize audio engine\n");
    // NOTE(lucas): Run the game without audio?
    game_run();
  }
  window_close();
  resources_unload();
  return 0;
}

void game_restart() {
  game_init(&game_state);
}

