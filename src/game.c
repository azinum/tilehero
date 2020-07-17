// game.c

#include "game_common.h"
#include "window.h"
#include "matrix_math.h"
#include "game.h"

typedef struct Game_state {
  u8 is_running;
} Game_state;

Game_state game_state;

static void game_state_init(Game_state* game);

void game_state_init(Game_state* game) {
  game->is_running = 1;
}

i32 game_execute(i32 window_width, i32 window_height, u8 fullscreen) {
  game_state_init(&game_state);
  if (window_open(window_width, window_height, fullscreen, "Tile Hero") != 0) {
    fprintf(stderr, "Failed to open window\n");
    return -1;
  }
  while (game_state.is_running) {
    if (window_process_input() != 0 || window_should_close()) {
      break;
    }
    window_pollevents();
    window_clear();
    window_swapbuffers();
  }
  window_close();
  return 0;
}
