// game.c

#include "common.h"
#include "window.h"
#include "matrix_math.h"
#include "audio_engine.h"
#include "entity.h"
#include "game.h"

#define ENTITIES_MAX (128)

typedef struct Game_state {
  Entity entities[ENTITIES_MAX];
  i32 entity_count;
  u8 is_running;
} Game_state;

Game_state game_state;

static void game_state_init(Game_state* game);
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

void game_state_init(Game_state* game) {
  game->is_running = 1;
  game->entity_count = 0;
  add_entity(128, 56, 16, 16);
}

void game_run() {
  while (game_state.is_running) {
    if (window_process_input() != 0 || window_should_close()) {
      break;
    }
    window_pollevents();
    
    for (i32 i = 0; i < game_state.entity_count; i++) {
      Entity* e = &game_state.entities[i];
      entity_render(e);
    }

    window_swapbuffers();
    window_clear();
  }
  window_close();
}

i32 game_execute(i32 window_width, i32 window_height, u8 fullscreen) {
  game_state_init(&game_state);
  if (window_open(window_width, window_height, fullscreen, "Tile Hero") != 0) {
    fprintf(stderr, "Failed to open window\n");
    return -1;
  }
  if (audio_engine_init(SAMPLE_RATE, FRAMES_PER_BUFFER, game_run) != 0) {
    fprintf(stderr, "Failed to initialize audio engine\n");
    return -1;
  }
  return 0;
}
