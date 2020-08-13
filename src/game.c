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

#define MAX_DELTA_TIME (0.2f)
#define USE_EDITOR 1
#define TIME_SCALING_MIN (0)
#define TIME_SCALING_MAX (10.0f)

Game_state game_state;

struct {
  i32 tile_type;
} editor = {
  .tile_type = TILE_NONE,
};

static float fade_value;
static i32 is_fading_out;

static void game_init(Game_state* game);
static void game_run();
static void game_editor_update();
static void editor_hud_render();
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
  game->time = 0;
  game->time_scale = 1;
  game->move_timer = 0;
  game->delta_time = 0;
  game->is_running = 1;
  game->mode = MODE_GAME;

#if 0
  for (i32 y = 1; y < TILE_COUNT_Y - 1; y++) {
    for (i32 x = 1; x < TILE_COUNT_X - 1; x++) {
      Entity* e = game_add_living_entity(x, y, TILE_SIZE, TILE_SIZE, 0, 1, 1, 1, 0);
      e->e_flags |= ENTITY_FLAG_FRIENDLY;
      e->e_flags ^= ENTITY_FLAG_DRAW_HEALTH;
      e->sprite_id = 2;
    }
  }
#endif
  is_fading_out = 1;
  fade_value = 1.0f;
  camera_init(-(window.width / 2), -(window.height / 2));
  tilemap_init(&game_state.tile_map, TILE_COUNT_X, TILE_COUNT_Y);
  audio_play_once_on_channel(SOUND_SONG_METAKING, 0, MUSIC_VOLUME);
}

void game_run() {
  game_init(&game_state);
  struct timeval time_now;
  struct timeval time_last;

  while (game_state.is_running && !window_process_input() && !window_should_close()) {
    time_last = time_now;
    gettimeofday(&time_now, NULL);
    game_state.delta_time = ((((time_now.tv_sec - time_last.tv_sec) * 1000000) + time_now.tv_usec) - (time_last.tv_usec)) / (1000000.0f);
    if (game_state.delta_time > MAX_DELTA_TIME)
      game_state.delta_time = MAX_DELTA_TIME;

    window_pollevents();
    camera_update();

    if (game_state.mode == MODE_GAME) {
      game_state.time += game_state.delta_time * game_state.time_scale;
    }
    if (key_pressed[GLFW_KEY_P]) {
      if (game_state.mode == MODE_GAME)
        game_state.mode = MODE_PAUSE;
      else
        game_state.mode = MODE_GAME;
    }

    if (key_pressed[GLFW_KEY_0]) {
      game_restart();
    }
    game_editor_update();

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

    if (game_state.time >= game_state.move_timer) {
      entity_do_tiled_move(game_state.entities, game_state.entity_count);
      game_state.move_timer = game_state.time + MOVE_INTERVAL;
    }

    tilemap_render(&game_state.tile_map);
    editor_hud_render();
    if (is_fading_out) {
      fade_out();
    }
    window_swapbuffers();
    window_clear();
  }
}

void game_editor_update() {
#if USE_EDITOR
  i32 x_tile = PIXEL_TO_TILE_POS(window.mouse_x + camera.x);
  i32 y_tile = PIXEL_TO_TILE_POS(window.mouse_y + camera.y);
  if (x_tile >= 0 && x_tile < TILE_COUNT_X && y_tile >= 0 && y_tile < TILE_COUNT_Y) {
    tilemap_render_tile_highlight(&game_state.tile_map, x_tile, y_tile);
  }
  if (key_pressed[GLFW_KEY_T]) {
    i32 x_tile = PIXEL_TO_TILE_POS(window.mouse_x + camera.x);
    i32 y_tile = PIXEL_TO_TILE_POS(window.mouse_y + camera.y);
    if (x_tile >= 0 && x_tile < TILE_COUNT_X && y_tile >= 0 && y_tile < TILE_COUNT_Y) {
      i16 health = 5 + rand() % 10;
      i16 attack = 1 + rand() % 3;
      game_add_living_entity(x_tile, y_tile, TILE_SIZE, TILE_SIZE, 0, 1, health, health, attack);
      audio_play_once(SOUND_0F, 0.5f);
    }
  }
  if (key_pressed[GLFW_KEY_Y]) {
    i32 x_tile = PIXEL_TO_TILE_POS(window.mouse_x + camera.x);
    i32 y_tile = PIXEL_TO_TILE_POS(window.mouse_y + camera.y);
    if (x_tile >= 0 && x_tile < TILE_COUNT_X && y_tile >= 0 && y_tile < TILE_COUNT_Y) {
      i16 health = 5 + rand() % 10;
      i16 attack = 1 + rand() % 3;
      Entity* e = game_add_living_entity(x_tile, y_tile, TILE_SIZE, TILE_SIZE, 1, 0, health, health, attack);
      e->sprite_id = 2;
      e->e_flags |= ENTITY_FLAG_FRIENDLY;
      e->e_flags ^= ENTITY_FLAG_DRAW_HEALTH;
      audio_play_once(SOUND_0F, 0.5f);
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

  if (key_pressed[GLFW_KEY_R]) {
    i32 x_tile = PIXEL_TO_TILE_POS(window.mouse_x + camera.x);
    i32 y_tile = PIXEL_TO_TILE_POS(window.mouse_y + camera.y);
    if (x_tile >= 0 && x_tile < TILE_COUNT_X && y_tile >= 0 && y_tile < TILE_COUNT_Y) {
      Tile* tile = tilemap_get_tile(&game_state.tile_map, x_tile, y_tile);
      if (tile) {
        tile->tile_type = editor.tile_type;
        audio_play_once(SOUND_0F, 0.5f);
      }
    }
  }
#endif
}

#define UI_TEXT_BUFF_SIZE (256)

void editor_hud_render() {
#if USE_EDITOR
  char ui_text[UI_TEXT_BUFF_SIZE] = {0};
{
  i32 x = 10;
  i32 y = 10;
  i32 w = TILE_SIZE * 2;
  i32 h = w;
  render_rect(x, y, 0.9f, w, h, 0.75f, 0.25f, 0.25f, 1, 0, 1.0f / TILE_SIZE);
  if (editor.tile_type != TILE_NONE) {
    render_texture_region(textures[TEXTURE_SPRITES],
              x, y, 0.9f, w, h, 0, (editor.tile_type + 4) * 8, 0, 8, 8);
  }
  else {
    render_filled_rect(x, y, 0.9f, w, h, 0, 0, 0, 1, 0);
  }
}
  i32 w = 230;
  i32 h = 140;
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
    ,
    (i32)camera.x, (i32)camera.y,
    window.width, window.height,
    game_state.time,
    (i32)(100 * game_state.time_scale),
    (i32)(1.0f / game_state.delta_time),
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

