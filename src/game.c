// game.c

#include "common.h"
#include "window.h"
#include "audio.h"
#include "entity.h"
#include "renderer.h"
#include "renderer_common.h"
#include "resource.h"
#include "editor.h"
#include "player.h"
#include "game.h"

#define MAX_DELTA_TIME (0.2f)

#define FADE_SPEED 2.0f

Game_state game_state;

static float fade_value;
static u8 fade_from_black;
static u8 is_fading;

static void game_init(Game_state* game);
static void game_run();
static void game_hud_render();
static void menu_update();
static void menu_render();
static void fade_out();
static void framebuffer_change_callback();

void game_entity_remove(Entity* e) {
  if (game_state.level.entity_count > 0) {
    Entity* top = &game_state.level.entities[--game_state.level.entity_count];
    if (e == camera.target) {
      camera.target = NULL;
    }
    if (top == camera.target) {
      camera.target = e;
    }
    if (game_state.level.entity_count == 0) {
      return;
    }
    *e = *top;
    return;
  }
  assert(0);
}

Entity* game_copy_add_entity(Entity copy) {
  Level* level = &game_state.level;
  if (level->entity_count >= MAX_ENTITY) {
    return NULL;
  }
  Entity* e = &level->entities[level->entity_count++];
  *e = copy;
  return e;
}

Entity* game_add_entity(float x, float y, float w, float h) {
  Entity* e = game_add_empty_entity();
  entity_init(e, x, y, w, h);
  return e;
}

Entity* game_add_empty_entity() {
  Level* level = &game_state.level;
  if (level->entity_count >= MAX_ENTITY) {
    return NULL;
  }
  Entity* e = &level->entities[level->entity_count++];
  entity_init(e, 0, 0, 0, 0);
  return e;
}

Entity* game_add_living_entity(i32 x_tile, i32 y_tile, float w, float h, i8 x_dir, i8 y_dir, i16 health, i16 max_health, i16 attack) {
  Entity* e = game_add_empty_entity();
  if (!e)
    return NULL;
  entity_init_tilepos(e, x_tile, y_tile, w, h);
  e->x_dir = x_dir;
  e->y_dir = y_dir;
  e->state = STATE_ACTIVE;
  e->e_flags |= ENTITY_FLAG_DRAW_HEALTH | ENTITY_FLAG_MOVABLE;
  e->health = health;
  e->max_health = max_health;
  e->attack = attack;
  return e;
}

void game_load_level(u32 index) {
  game_fade_from_black();
  move_count = 0;
  camera.target = NULL;
  level_load(&game_state.level, index);
}

void game_fade_to_black() {
  is_fading = 1;
  fade_value = 0;
  fade_from_black = 0;
}

void game_fade_from_black() {
  is_fading = 1;
  fade_value = 1;
  fade_from_black = 1;
}

void game_init(Game_state* game) {
  srand((u32)time(NULL));
  game->time = 0;
  game->time_scale = 1;
  game->delta_time = 0;
  game->move_timer = 0;
  game->should_move = 0;
  game->is_running = 1;
  game->mode = MODE_GAME;

  memset(&game->level, 0, sizeof(Level));

  game_fade_from_black();
  camera_init(0, 0);
  move_count = 0;
  move_time = 0;

  game_load_level(0);
  audio_play_once_on_channel(SOUND_SONG_METAKING, 0, MUSIC_VOLUME);
}

void game_run() {
  Game_state* game = &game_state;
  game_init(game);
  ui_init();
  struct timeval time_now = {0};
  struct timeval time_last = {0};

  while (game_state.is_running && !window_process_input() && !window_should_close()) {
    time_last = time_now;
    gettimeofday(&time_now, NULL);
    game->delta_time = ((((time_now.tv_sec - time_last.tv_sec) * 1000000) + time_now.tv_usec) - (time_last.tv_usec)) / (1000000.0f);
    if (game->delta_time > MAX_DELTA_TIME)
      game->delta_time = MAX_DELTA_TIME;

    window_pollevents();
    switch (game->mode) {
      case MODE_GAME: {
        game->time += game->delta_time * game->time_scale;
        break;
      }
      case MODE_PAUSE:
        break;
      case MODE_EDITOR: {
        editor_update(game);
        editor_render(game);
        break;
      }
    }

    if (game->mode == MODE_MENU) {
      menu_update();
      menu_render();
    }
    else {
      if (key_pressed[GLFW_KEY_P]) {
        if (game->mode == MODE_GAME)
          game->mode = MODE_PAUSE;
        else
          game->mode = MODE_GAME;
      }
      if (key_pressed[GLFW_KEY_F1]) {
        if (game->mode == MODE_EDITOR) {
          game->mode = MODE_GAME;
        }
        else {
          game->mode = MODE_EDITOR;
        }
      }

      if (key_pressed[GLFW_KEY_0]) {
        game_restart();
      }

      if (key_pressed[GLFW_KEY_ESCAPE]) {
        game->mode = MODE_MENU;
      }

      camera_update();
      player_controller();

      for (u32 i = 0; i < game->level.entity_count; i++) {
        Entity* e = &game->level.entities[i];
        if (game->mode == MODE_GAME) {
          if (e->e_flags & ENTITY_FLAG_PLAYER) {
            player_update(e);
            if (!camera.target) {
              camera.target = e;
            }
          }
          entity_update(e);
        }
        entity_render(e);
      }

      if (game->should_move) {
        entity_do_tiled_move(game->level.entities, game->level.entity_count, &game->level);
        game->should_move = 0;
      }

      game_hud_render();

      tilemap_render(&game->level.tile_map);
    }
    ui_render();

    if (is_fading)
      fade_out();
    window_swapbuffers();
    window_clear();
  }
}

void menu_update() {
  Game_state* game = &game_state;

  if (key_pressed[GLFW_KEY_ENTER]) {
    game->mode = MODE_GAME;
  }
  if (key_pressed[GLFW_KEY_ESCAPE]) {
    game->is_running = 0;
  }
}

static char ui_text[UI_TEXT_BUFFER_SIZE] = {0};

#define render_simple_menu_text(text, x, y, font_size) \
  snprintf(ui_text, UI_TEXT_BUFFER_SIZE, text); \
  render_simple_text(textures[TEXTURE_FONT], \
    x, y, 0.9f, \
    500, 500, \
    font_size, \
    0.7f, 0.7f, 12.0f, \
    ui_text, UI_TEXT_BUFFER_SIZE)

void menu_render() {
  ui_focus(UI_MAIN_MENU);
  struct UI_element* e = NULL;

  ui_do_button(UI_ID, window.width - (16 * 16), 16 * 1, 16 * 15, 16 * 8, "INFO: You have (2) unread messages.\n\nClick here to read them.", 16, &e);
  UI_INIT(e,
    e->movable = 0;
    e->font_color = COLOR_MESSAGE;
  );

  if (ui_do_button(UI_ID, 16 * 1, window.height - (16 * 8), 16 * 9, 16 * 3, "Resume game", 24, &e)) {
    game_state.mode = MODE_GAME;
    return;
  }
  UI_INIT(e,
    e->border = 0;
    e->background_color = COLOR_OK;
  );

  if (ui_do_button(UI_ID, 16 * 1, window.height - (16 * 4), 16 * 9, 16 * 3, "Quit", 24, &e)) {
    game_state.is_running = 0;
    return;
  }
  UI_INIT(e,
    e->border = 0;
    e->background_color = COLOR_WARN;
  );

  render_simple_menu_text("Tile Hero", VW(1), VW(1), 42);
}

void game_hud_render() {
  Game_state* game = &game_state;

  ui_focus(UI_DEFAULT);
  struct UI_element* e = NULL;

  if (ui_do_button(UI_ID, VW(2), 16, 16 * 12, 16 * 3, "Restart", 24, &e)) {
    game_restart();
  }
  UI_INIT(e,
    e->background_color = COLOR_WARN;
    e->border = 0;
  );

  if (ui_do_button(UI_ID, VW(2), 16 * 5, 16 * 12, 16 * 3, "Next level", 24, &e)) {
    game_load_level(game->level.index + 1);
  }
  UI_INIT(e,
    e->background_color = COLOR_OK;
    e->border = 0;
  );

  if (ui_do_button(UI_ID, VW(2), 16 * 9, 16 * 12, 16 * 3, "Prev level", 24, &e)) {
    if (game->level.index > 0) {
      game_load_level(game->level.index - 1);
    }
  }
  UI_INIT(e,
    e->background_color = COLOR_ACCEPT;
    e->border = 0;
  );

  audio_engine.muted = ui_do_checkbox(UI_ID, VW(2), 16 * 13, 32, 32, audio_engine.muted, NULL, 0, NULL);
  render_simple_text(textures[TEXTURE_FONT], VW(2) + 16 * 2, 16 * 13, 0.9f, 11 * 16, 3 * 16, 12, 0.7f, 0.7f, 12.0f, "audio muted", -1);

  camera.has_target = ui_do_checkbox(UI_ID, VW(2), 16 * 16, 32, 32, camera.has_target, NULL, 0, NULL);
  render_simple_text(textures[TEXTURE_FONT], VW(2) + 16 * 2, 16 * 16, 0.9f, 11 * 16, 3 * 16, 12, 0.7f, 0.7f, 12.0f, "camera has target", -1);

  snprintf(ui_text, UI_TEXT_BUFFER_SIZE,
    "entity count: %i/%i\n"
    "fps: %i\n"
    "time: %.3f\n"
    "time scale: %i %%\n"
    "level: %i\n"
    ,
    game->level.entity_count, MAX_ENTITY,
    (i32)(1.0f / game->delta_time),
    game->time,
    (i32)(100 * game->time_scale),
    game->level.index
  );
  ui_do_text(UI_ID, VW(2), 16 * 19, 16 * 16, 16 * 9, ui_text, 16, NULL);
}

void fade_out() {
  if (fade_from_black) {  // Fade from black
    fade_value = lerp(fade_value, 0.0f, FADE_SPEED * game_state.delta_time);
    if (fade_value < 0.01f) {
      fade_value = 0;
      is_fading = 0;
    }
  }
  else {  // Fade to black
    fade_value = lerp(fade_value, 1.0f, FADE_SPEED * game_state.delta_time);
    if (fade_value > (1 - 0.01f)) {
      fade_value = 1;
      is_fading = 0;
    }
  }
  render_filled_rect(0, 0, 1, window.width, window.height, 0, 0, 0, fade_value, 0);
}

void framebuffer_change_callback() {
  ui_focus(0);
}

i32 game_execute(i32 window_width, i32 window_height, u8 fullscreen) {
  log_file = fopen(LOG_FILE, "w");
  if (window_open(window_width, window_height, fullscreen, "Tile Hero", framebuffer_change_callback) != 0) {
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
  renderer_free();
  resources_unload();
  if (log_file != stdout) {
    fclose(log_file);
  }
  return 0;
}

void game_restart() {
  // game_init(&game_state);
  game_load_level(game_state.level.index);
}

