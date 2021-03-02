// game.c

#include <stdarg.h>

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

#define MAX_DELTA_TIME (0.3f)

#define FADE_SPEED 2.5f
#define MESSAGE_LENGTH 64
#define MAX_MESSAGE 12
#define MESSAGE_TIME 2.0f // How fast we empty the message queue
#define MAX_EVENT 32

Game_state game_state;

static char messages[MAX_MESSAGE][MESSAGE_LENGTH] = {0};
static i32 message_count = 0;
static float last_message_time = 0.0f;
static float fade_value;
static u8 fade_from_black;
static u8 is_fading;
static Event events[MAX_EVENT] = {0};
static i32 event_count = 0;

static void event_kill(Entity* e, Event_arg arg);

void event_kill(Entity* e, Event_arg arg) {
  (void)arg;
  e->state = STATE_DEAD;
}

static void game_init(Game_state* game);
static void game_run();
static void game_hud_render();
static void menu_update();
static void menu_render();
static void messages_update();
static void messages_render();
static void fade_out();
static void framebuffer_change_callback();

void game_entity_remove(Entity* e) {
  if (game_state.world.entity_count > 0) {
    Entity* top = &game_state.world.entities[--game_state.world.entity_count];
    if (e == camera.target) {
      camera.target = NULL;
    }
    if (top == camera.target) {
      camera.target = e;
    }
    if (game_state.world.entity_count == 0) {
      return;
    }
    *e = *top;
    return;
  }
  assert(0);
}

Entity* game_copy_add_entity(Entity copy) {
  World* world = &game_state.world;
  if (world->entity_count >= MAX_ENTITY) {
    return NULL;
  }
  Entity* e = &world->entities[world->entity_count++];
  *e = copy;
  return e;
}

Entity* game_add_entity(float x, float y, float w, float h) {
  Entity* e = game_add_empty_entity();
  entity_init(e, x, y, w, h);
  return e;
}

Entity* game_add_empty_entity() {
  World* world = &game_state.world;
  if (world->entity_count >= MAX_ENTITY) {
    return NULL;
  }
  Entity* e = &world->entities[world->entity_count++];
  return e;
}

Entity* game_add_living_entity(i32 x_tile, i32 y_tile, float w, float h, i8 x_dir, i8 y_dir, i16 health, i16 max_health, i16 attack) {
  Entity* e = game_add_empty_entity();
  if (!e)
    return NULL;
  entity_init(e, 0, 0, w, h);
  entity_init_tilepos(e, x_tile, y_tile);
  e->x_dir = x_dir;
  e->y_dir = y_dir;
  e->state = STATE_ACTIVE;
  e->e_flags |= ENTITY_FLAG_DRAW_HEALTH | ENTITY_FLAG_MOVABLE;
  e->health = health;
  e->max_health = max_health;
  e->attack = attack;
  return e;
}

i32 game_load_level(i32 index) {
  if (index >= 0) {
    game_fade_from_black();
    move_count = 0;
    camera.target = NULL;
    level_load(&game_state.world, index);
    return NO_ERR;
  }
  return ERR;
}

i32 game_load_level_on_complete(i32 index) {
  i32 result = game_load_level(index);
  if (result == NO_ERR) {
    game_state.save_state.level = index;
    save_state_store(&game_state.save_state);
  }
  return result;
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
  game->total_time = 0;
  game->time_scale = 1;
  game->delta_time = 0;
  game->move_timer = 0;
  game->should_move = 0;
  game->is_running = 1;
  game->mode = MODE_GAME;

  memset(&game->world, 0, sizeof(World));

  game_fade_from_black();
  camera_init(0, 0);
  move_count = 0;
  move_time = 0;

#if 0
  save_state_load(&game->save_state);
  game_load_level(game->save_state.level);
#else
  game_load_level(0);
#endif
  // audio_play_once_on_channel(SOUND_SONG_REMADE, 1, MUSIC_VOLUME);
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

    game->total_time += game->delta_time * game->time_scale;

    window_pollevents();
    switch (game->mode) {
      case MODE_GAME: {
        game->time += game->delta_time * game->time_scale;
        game_hud_render();
        break;
      }
      case MODE_PAUSE:
        break;
      case MODE_EDITOR: {
        editor_update(game);
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
          camera.has_target = 1;
        }
        else {
          game->mode = MODE_EDITOR;
          camera.has_target = 0;
        }
      }

      if (key_pressed[GLFW_KEY_R] || key_pressed[GLFW_KEY_0]) {
        game_restart();
        game_send_message("Loaded level %i", game->world.level.index);
      }
      if (key_pressed[GLFW_KEY_K]) {
        game_create_event(ENTITY_TYPE_NPC, 0, event_kill, (Event_arg) {0});
        game_send_message("You used an almighty force to kill all enemies");
      }

      if (key_pressed[GLFW_KEY_ESCAPE]) {
        game->mode = MODE_MENU;
      }

      camera_update();
      player_controller();

      u8 did_events = 0;
      for (u32 i = 0; i < game->world.entity_count; i++) {
        Entity* e = &game->world.entities[i];
        if (game->mode == MODE_GAME) {
          if (e->e_flags & ENTITY_FLAG_PLAYER) {
            player_update(e);
            if (!camera.target) {
              camera.target = e;
            }
          }
          entity_update(e);
          if (event_count > 0) {
            for (i32 event_index = 0; event_index < event_count; event_index++) {
              Event* event = &events[event_index];
              if (!event->func) {
                continue;
              }
              if (event->target_type == e->type || event->target_type == EVENT_TARGET_TYPE_ANY) {
                // Do event only if target flags (a) is a subset of entity flags (b)
                // (a | b) == b
                if ((event->target_flags | e->e_flags) == e->e_flags || event->target_flags == 0) {
                  event->func(e, event->arg);
                }
              }
              did_events = 1;
            }
          }
        }
        entity_render(e);
      }
      if (did_events) {
        event_count = 0;
      }

      if (game->should_move) {
        entity_do_tiled_move(game->world.entities, game->world.entity_count, &game->world.level);
        game->should_move = 0;
      }

      tilemap_render(&game->world.level.tile_map);
    }

    messages_update();
    messages_render();
    ui_update();
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

  if (ui_do_button(UI_ID, VW(2), 16, 16 * 13, 16 * 3, "Restart [r]", 24, &e)) {
    game_restart();
    game_send_message("Loaded level %i", game->world.level.index);
  }
  UI_INIT(e,
    e->background_color = COLOR_WARN;
    e->border = 0;
  );

  if (ui_do_button(UI_ID, VW(2), 16 * 5, 16 * 13, 16 * 3, "Prev level", 24, &e)) {
    if (game_load_level(game->world.level.index - 1) == NO_ERR) {
      game_send_message("Loaded level %i", game->world.level.index);
    }
  }
  UI_INIT(e,
    e->background_color = COLOR_OK;
    e->border = 0;
  );

  if (ui_do_button(UI_ID, VW(2), 16 * 9, 16 * 13, 16 * 3, "Next level", 24, &e)) {
    game_load_level(game->world.level.index + 1);
    game_send_message("Loaded level %i", game->world.level.index);
  }
  UI_INIT(e,
    e->background_color = COLOR_ACCEPT;
    e->border = 0;
  );
}

void messages_update() {
  struct Game_state* game = &game_state;
  if (game->total_time >= last_message_time + MESSAGE_TIME) {
    message_count = 0;
  }
}

void messages_render() {
  i32 text_size = 14;
  i32 x_pos = window.width >> 1;
  i32 y_pos = VW(1);
  for (i32 i = 0; i < message_count; i++) {
    char* message = (char*)&messages[i];
    render_simple_text(textures[TEXTURE_FONT], x_pos, y_pos, 0.9f, text_size * MESSAGE_LENGTH, text_size * 1.5f, text_size, 0.7f, 0.7f, 0.0f /* margin */, message, MESSAGE_LENGTH);
    y_pos += text_size * 1.5f;
  }
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
  renderer_on_update_framebuffer();
  ui_focus(0);
}

void game_send_message(char* fmt, ...) {
  for (i32 i = message_count; i > 0; i--) {
    if (i < MAX_MESSAGE) {
      strncpy(messages[i], messages[i - 1], MESSAGE_LENGTH);
    }
  }
  va_list args;
  va_start(args, fmt);
  char* buff = (char*)&messages[0];
  vsnprintf(buff, MESSAGE_LENGTH, fmt, args);
  last_message_time = game_state.total_time;
  if (message_count < MAX_MESSAGE) {
    message_count++;
  }
  va_end(args);
}

i32 game_create_event(i32 type, i32 flags, event_func func, Event_arg arg) {
  Event e = (Event) {
    .target_type = type,
    .target_flags = flags,
    .func = func,
    .arg = arg,
  };
  if (event_count < MAX_EVENT) {
    events[event_count++] = e;
    return NO_ERR;
  }
  return ERR;
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
  game_load_level(game_state.world.level.index);
}

