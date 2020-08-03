// game.c

#include "common.h"
#include "window.h"
#include "matrix_math.h"
#include "audio.h"
#include "entity.h"
#include "renderer.h"
#include "resource.h"
#include "game.h"

Game_state game_state;

static void game_init(Game_state* game);
static void game_run();
static Entity* add_entity(float x, float y, float w, float h);

Entity* add_entity(float x, float y, float w, float h) {
  Entity* e = NULL;
  if (game_state.entity_count >= ENTITIES_MAX)
    return NULL;
  e = &game_state.entities[game_state.entity_count++];
  entity_init(e, x, y, w, h);
  return e;
}

inline float random_number(float from, float to) {
  return (float)rand() / (float)(RAND_MAX / to) + (float)rand() / (float)(RAND_MAX / from);
}

void game_init(Game_state* game) {
  srand((u32)time(NULL));
  game->is_running = 1;
  game->entity_count = 0;
  for (i32 i = 0; i < 16; i++) {
    Entity* e = add_entity(32 * (rand() % 32), 32 * i, 32, 32);
    while (!e->x_speed) {
      e->x_speed = random_number(-3, 3);
    }
    while (!e->y_speed) {
      e->y_speed = random_number(-3, 3);
    }
  }
  camera_init(0, 0);
}

void game_run() {
  while (game_state.is_running) {
    window_pollevents();
    if (window_process_input() != 0 || window_should_close()) {
      break;
    }
    game_state.tick++;

    render_rect(window.mouse_x - 12, window.mouse_y - 12, 0.1f, 24, 24, 0.1f, 0.85f, 0.22f, 0, 5.0f / 24);
    for (i32 i = 0; i < game_state.entity_count; i++) {
      Entity* e = &game_state.entities[i];
      entity_update(e);
      entity_render(e);
    }
    render_rect(0 - camera.x, 0 - camera.y, 0.1f, 250 + 32, 250 + 32, 0.3f, 0.85f, 0.2f, 0, 1 / 250.0f);

    window_swapbuffers();
    window_clear();
  }
  window_close();
}

i32 game_execute(i32 window_width, i32 window_height, u8 fullscreen) {
  if (window_open(window_width, window_height, fullscreen, "Tile Hero") != 0) {
    fprintf(stderr, "Failed to open window\n");
    return -1;
  }
  resources_load();
  game_init(&game_state);
  if (audio_engine_init(SAMPLE_RATE, FRAMES_PER_BUFFER, game_run) != 0) {
    fprintf(stderr, "Failed to initialize audio engine\n");
    // NOTE(lucas): Run the game without audio?
    game_run();
  }
  return 0;
}
