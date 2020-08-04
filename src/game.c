// game.c

#include "common.h"
#include "window.h"
#include "matrix_math.h"
#include "audio.h"
#include "entity.h"
#include "renderer.h"
#include "resource.h"
#include "game.h"

#define mouse_over(M_X, M_Y, X, Y, W, H) (M_X >= X && M_X <= X + W && M_Y >= Y && M_Y <= Y + H)

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
  for (i32 i = 0; i < 5; i++) {
    Entity* e = add_entity(32 * (rand() % 32), 32 * i, 32, 32);
    while (!e->x_speed) {
      e->x_speed = random_number(-2, 2);
    }
    while (!e->y_speed) {
      e->y_speed = random_number(-2, 2);
    }
  }
  camera_init(0, 0);
}

void game_run() {
  resources_load();
  while (game_state.is_running) {
    window_pollevents();
    if (window_process_input() != 0 || window_should_close()) {
      break;
    }
    game_state.tick++;
    camera_update();

    for (i32 i = 0; i < game_state.entity_count; i++) {
      Entity* e = &game_state.entities[i];
      entity_update(e);
      entity_render(e);
      if (mouse_over(window.mouse_x + camera.x, window.mouse_y + camera.y, e->x, e->y, e->w, e->h)) {
        entity_render_highlight(e);
      }
    }
    render_rect(0 - camera.x, 0 - camera.y, 0.1f, 250 + 32, 250 + 32, 0.3f, 0.85f, 0.2f, 0, 1 / 250.0f);

    window_swapbuffers();
    window_clear();
  }
  window_close();
  resources_unload();
}

i32 game_execute(i32 window_width, i32 window_height, u8 fullscreen) {
  if (window_open(window_width, window_height, fullscreen, "Tile Hero") != 0) {
    fprintf(stderr, "Failed to open window\n");
    return -1;
  }
  game_init(&game_state);
  if (audio_engine_init(SAMPLE_RATE, FRAMES_PER_BUFFER, game_run) != 0) {
    fprintf(stderr, "Failed to initialize audio engine\n");
    // NOTE(lucas): Run the game without audio?
    game_run();
  }
  return 0;
}
