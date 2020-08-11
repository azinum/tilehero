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

static float fade_value = 1.0f;
static i32 is_fading_out = 1;

static void game_init(Game_state* game);
static void game_run();
static void dev_hud_render();
static void fade_out();
static Entity* add_entity(float x, float y, float w, float h);
static Entity* add_empty_entity();
static Entity* add_living_entity(i32 x_tile, i32 y_tile, float w, float h, i16 health, i16 max_health);

Entity* add_entity(float x, float y, float w, float h) {
  Entity* e = add_empty_entity();
  entity_init(e, x, y, w, h);
  return e;
}

Entity* add_empty_entity() {
  if (game_state.entity_count >= ENTITIES_MAX)
    return NULL;
  return &game_state.entities[game_state.entity_count++];
}

Entity* add_living_entity(i32 x_tile, i32 y_tile, float w, float h, i16 health, i16 max_health) {
  Entity* e = add_empty_entity();
  if (!e)
    return NULL;
  entity_init_tilepos(e, x_tile, y_tile, w, h);
  if (x_tile > 6) {
    e->x_dir = 1;
  }
  else {
    e->y_dir = 1;
  }
  e->state = STATE_ALIVE;
  e->e_flags |= ENTITY_FLAG_DRAW_HEALTH;
  e->health = health;
  e->max_health = max_health;
  return e;
}

void game_init(Game_state* game) {
  srand((u32)time(NULL));
  (void)add_entity;
  game->is_running = 1;
  game->entity_count = 0;

  for (u32 i = 0; i < 8; i++) {
    add_living_entity(i, i, 32, 32, 5, 5);
  }

  camera_init(-(window.width / 2), -(window.height / 2));
  tilemap_init(&game_state.tile_map, TILE_COUNT_X, TILE_COUNT_Y);
  audio_play_once_on_channel(SOUND_SONG_METAKING, 0, 0.2f);
}

void game_run() {
  game_init(&game_state);
  while (game_state.is_running && !window_process_input() && !window_should_close()) {
    window_pollevents();
    game_state.tick++;

    if (key_pressed[GLFW_KEY_T]) {
      i32 x_tile = (i32)((window.mouse_x + camera.x) / TILE_SIZE);
      i32 y_tile = (i32)((window.mouse_y + camera.y) / TILE_SIZE);
      if (x_tile >= 0 && x_tile < TILE_COUNT_X && y_tile >= 0 && y_tile < TILE_COUNT_Y) {
        add_living_entity(x_tile, y_tile, TILE_SIZE, TILE_SIZE, 5, 5);
        audio_play_once(SOUND_0F, 0.1f);
      }
    }

    camera_update();

    for (i32 i = 0; i < game_state.entity_count; i++) {
      Entity* e = &game_state.entities[i];
      entity_update_and_render(e);
      if (e->state == STATE_NONE)
        continue;
      if (mouse_over(window.mouse_x + camera.x, window.mouse_y + camera.y, e->x, e->y, e->w, e->h)) {
        entity_render_highlight(e);
        if (key_pressed[GLFW_KEY_F]) {
          camera.target = e;
          camera.has_target = 1;
        }
      }
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

  snprintf(ui_text, UI_TEXT_BUFF_SIZE, "camera x: %i, y: %i\nwindow size: %ix%i\ntick: %i\nentity count: %i/%i", (i32)camera.x, (i32)camera.y, window.width, window.height, game_state.tick, game_state.entity_count, ENTITIES_MAX);
#if 1
  render_text(textures[TEXTURE_FONT],
    10, window.height - 10 - 90, // x, y
    0.9f, // z
    230,   // Width
    90, // Height
    12, // Font size
    0.7f, // Font kerning
    0.7f, // Line spacing
    12.0f, // Margin
    ui_text,
    UI_TEXT_BUFF_SIZE
  );

  if (camera.has_target) {
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
