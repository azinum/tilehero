// game.c

#include "common.h"
#include "window.h"
#include "matrix_math.h"
#include "audio_engine.h"
#include "entity.h"
#include "renderer.h"
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

void game_init(Game_state* game) {
  game->is_running = 1;
  game->entity_count = 0;
  for (i32 i = 0; i < 16; i++) {
    Entity* e = add_entity(32 * (rand() % 32), 32 * i, 64, 64);
    while (!e->x_speed) {
      e->x_speed = (rand() % 2) - (rand() % 2);
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

    window_swapbuffers();
    window_clear();
  }
  window_close();
}

i32 game_execute(i32 window_width, i32 window_height, u8 fullscreen) {
  game_init(&game_state);
  if (window_open(window_width, window_height, fullscreen, "Tile Hero") != 0) {
    fprintf(stderr, "Failed to open window\n");
    return -1;
  }
  // NOTE(lucas): The audio engine will not invoke the callback function in case initialization fails.
  if (audio_engine_init(SAMPLE_RATE, FRAMES_PER_BUFFER, game_run) != 0) {
    fprintf(stderr, "Failed to initialize audio engine\n");
    // NOTE(lucas): Run the game without audio? Maybe an device is missing,
    // if that's the case then we'd might want to be checking for devices
    // while the game is running?
    game_run();
    return 0;
  }
  return 0;
}
