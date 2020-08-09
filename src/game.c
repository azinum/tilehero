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
static void dev_hud_render();
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
  for (i32 i = 0; i < 13; i++) {
    Entity* e = add_entity(3 * (rand() % 32), 4 * i, 32, 32);
    while (!e->x_speed) {
      e->x_speed = random_number(-1, 1);
    }
    while (!e->y_speed) {
      e->y_speed = random_number(-1, 1);
    }
  }
  camera_init(0, 0);
}


void game_run() {
  resources_load();
  game_init(&game_state);
  while (game_state.is_running) {
    window_pollevents();
    if (window_process_input() != 0 || window_should_close()) {
      break;
    }
    game_state.tick++;
    camera_update();
    render_rect(0 - camera.x, 0 - camera.y, 0, 250 + 32, 250 + 32, 0.3f, 0.85f, 0.2f, 1.0f, 0, 0.004f);

    for (i32 i = 0; i < game_state.entity_count; i++) {
      Entity* e = &game_state.entities[i];
      entity_update(e);
      entity_render(e);
      if (mouse_over(window.mouse_x + camera.x, window.mouse_y + camera.y, e->x, e->y, e->w, e->h)) {
        entity_render_highlight(e);
      }
    }

    dev_hud_render();
    window_swapbuffers();
    window_clear();
  }
  window_close();
  resources_unload();
}

#define UI_TEXT_BUFF_SIZE (256)

void dev_hud_render() {
  char some_text[UI_TEXT_BUFF_SIZE] = {0};

  // snprintf(some_text, UI_TEXT_BUFF_SIZE, "Camera x: %i, y: %i\nWin w: %i, h: %i", (i32)camera.x, (i32)camera.y, window.width, window.height);
  snprintf(some_text, UI_TEXT_BUFF_SIZE, "camera.x: %i, camera.y: %i\n\nwindow.width: %i, window.height: %i, game_state.tick: %i, asldalsd aksld lkajsdkljlkjslajda sdkjalskjdjk THE END", (i32)camera.x, (i32)camera.y, window.width, window.height, game_state.tick);
#if 0
  render_text(textures[TEXTURE_FONT],
    10.0f, 10.0f, // x, y
    0.9f, // z
    200,   // Width
    400 - (100 * (1 + sin(game_state.tick / 100.0f))),  // Height
    16, // Font size
    0.7f, // Font kerning
    // 25.0f * (1 + sin(game_state.tick / 80.0f)), // Margin
    25.0f, // Margin
    some_text,
    UI_TEXT_BUFF_SIZE
  );
#endif

  render_text(textures[TEXTURE_FONT],
    10.0f, 10.0f, // x, y
    0.9f, // z
    window.mouse_x,
    window.mouse_y,
    20 - (4 * (1 + sin(game_state.tick / 100.0f))), // Font size
    0.7f, // Font kerning
    10.0f, // Margin
    some_text,
    UI_TEXT_BUFF_SIZE
  );
}

i32 game_execute(i32 window_width, i32 window_height, u8 fullscreen) {
  if (window_open(window_width, window_height, fullscreen, "Tile Hero") != 0) {
    fprintf(stderr, "Failed to open window\n");
    return -1;
  }
  if (audio_engine_init(SAMPLE_RATE, FRAMES_PER_BUFFER, game_run) != 0) {
    fprintf(stderr, "Failed to initialize audio engine\n");
    // NOTE(lucas): Run the game without audio?
    game_run();
  }
  return 0;
}
