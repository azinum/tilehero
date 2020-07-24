// game.c

#include "common.h"
#include "window.h"
#include "matrix_math.h"
#include "audio_engine.h"
#include "game.h"

typedef struct Game_state {
  u8 is_running;
} Game_state;

Game_state game_state;

static void game_state_init(Game_state* game);
static void game_run();

void game_state_init(Game_state* game) {
  game->is_running = 1;
}

void game_run() {
  while (game_state.is_running) {
    if (window_process_input() != 0 || window_should_close()) {
      break;
    }
    window_pollevents();
    window_clear();
    window_swapbuffers();
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
