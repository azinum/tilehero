// game.c

#include "common.h"
#include "window.h"
#include "matrix_math.h"
#include "audio.h"
#include "entity.h"
#include "renderer.h"
#include "renderer_common.h"
#include "resource.h"
#include "editor.h"
#include "player.h"
#include "game.h"

#define MAX_DELTA_TIME (0.2f)

Game_state game_state;

static float fade_value;
static i32 is_fading_out;

static void game_init(Game_state* game);
static void game_run();
static void fade_out();

Entity* game_add_entity(float x, float y, float w, float h) {
  Entity* e = game_add_empty_entity();
  entity_init(e, x, y, w, h);
  return e;
}

Entity* game_add_empty_entity() {
  World* world = &game_state.world;
  if ((world->chunk->entity_count) >= MAX_ENTITY) {
    return NULL;
  }
  Entity* e = &world->chunk->entities[world->chunk->entity_count++];
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

void game_init(Game_state* game) {
  srand((u32)time(NULL));
  game->time = 0;
  game->time_scale = 1;
  game->delta_time = 0;
  game->move_timer = 0;
  game->should_move = 0;
  game->is_running = 1;
  game->mode = MODE_GAME;

  memset(&game->world, 0, sizeof(struct World));

  is_fading_out = 1;
  fade_value = 1.0f;
  camera_init((window.width / 2), (window.height / 2));
  world_load_chunks_from_origin(&game->world, WORLD_VEC3(1, 1, 0));

  move_count = 0;
  move_time = 0;

  audio_play_once_on_channel(SOUND_SONG_METAKING, 0, MUSIC_VOLUME);
}

void game_run() {
  game_init(&game_state);
  struct timeval time_now = {0};
  struct timeval time_last = {0};

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

    for (u32 i = 0; i < game_state.world.chunk->entity_count; i++) {
      Entity* e = &game_state.world.chunk->entities[i];
      if (game_state.mode == MODE_GAME) {
        if (e->e_flags & ENTITY_FLAG_PLAYER) {
          player_update(e);
          if (!camera.target) {
            camera.target = e;
            camera.has_target = 1;
          }
        }
        entity_update(e);
      }
      entity_render(e);
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
      World_position p = WORLD_VEC3((i32)(e->x_tile / TILE_COUNT_X), (i32)(e->y_tile / TILE_COUNT_Y), 0);
      World_position center = game_state.world.current_origin;
      if (!VEC3I_EQUAL(p, center)) {
        world_add_entity_to_swap(&game_state.world, e);
        game_entity_remove(e);
      }
    }

    editor_update();
    editor_render();

    for (u32 i = 0; i < NUM_LOADED_WORLD_CHUNKS; i++) {
      World_chunk* chunk = &game_state.world.chunks[i];
      assert(chunk);
      World_position position = chunk->position;
      tilemap_render(&chunk->tile_map, position);
    }

    if (game_state.should_move) {
      entity_do_tiled_move(game_state.world.chunk->entities, game_state.world.chunk->entity_count);
      game_state.should_move = 0;
    }

    if (is_fading_out) {
      fade_out();
    }
    window_swapbuffers();
    window_clear();
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
  if (game_state.world.chunk->entity_count > 0) {
    Entity* top = &game_state.world.chunk->entities[--game_state.world.chunk->entity_count];
    if (e == camera.target) {
      camera.target = NULL;
      camera.has_target = 0;
    }
    if (top == camera.target) {
      camera.target = e;
    }
    if (game_state.world.chunk->entity_count == 0) {
      return;
    }
    *e = *top;
    return;
  }
  assert(0);
}

i32 game_execute(i32 window_width, i32 window_height, u8 fullscreen) {
  log_file = fopen(LOG_FILE, "w");
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
  world_chunks_store_hashed(&game_state.world, WORLD_STORAGE_FILE);
  window_close();
  renderer_free();
  resources_unload();
  if (log_file != stdout) {
    fclose(log_file);
  }
  return 0;
}

void game_restart() {
  game_init(&game_state);
}

