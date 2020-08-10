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
static Entity* add_empty_entity();

Entity* add_entity(float x, float y, float w, float h) {
  Entity* e = add_empty_entity();
  entity_init(e, x, y, w, h);
  return e;
}

Entity* add_empty_entity() {
  if (game_state.entity_count >= ENTITIES_MAX)
    return NULL;
  return &game_state.entities[game_state.entity_count++];
}

void game_init(Game_state* game) {
  srand((u32)time(NULL));
  (void)add_entity;
  game->is_running = 1;
  game->entity_count = 0;

  for (u32 i = 0; i < 12; i++) {
    Entity* e = add_empty_entity();
    entity_init_tilepos(e, i, i, 32, 32);
    e->x_dir = 1;
  }

  camera_init(-(window.width / 2), -(window.height / 2));
  tilemap_init(&game_state.tile_map, TILE_COUNT_X, TILE_COUNT_Y);
  audio_play_once_on_channel(SOUND_SONG_METAKING, 0, 0.05f);
}

void game_run() {
  game_init(&game_state);
  while (game_state.is_running) {
    window_pollevents();
    if (window_process_input() != 0 || window_should_close()) {
      break;
    }
    game_state.tick++;
    camera_update();

    tilemap_render(&game_state.tile_map);

    for (i32 i = 0; i < game_state.entity_count; i++) {
      Entity* e = &game_state.entities[i];
      entity_update_and_render(e);
      if (mouse_over(window.mouse_x + camera.x, window.mouse_y + camera.y, e->x, e->y, e->w, e->h)) {
        entity_render_highlight(e);
      }
    }

    dev_hud_render();
    window_swapbuffers();
    window_clear();
  }
}

#define UI_TEXT_BUFF_SIZE (256)

void dev_hud_render() {
  char some_text[UI_TEXT_BUFF_SIZE] = {0};

  snprintf(some_text, UI_TEXT_BUFF_SIZE, "camera.x: %i, camera.y: %i\n\nwindow.width: %i, window.height: %i, game_state.tick: %i, asldalsd aksld lkajsdkljlkjslajda sdkjalskjdjk THE END", (i32)camera.x, (i32)camera.y, window.width, window.height, game_state.tick);
#if 1
  render_text(textures[TEXTURE_FONT],
    10.0f, 10.0f, // x, y
    0.9f, // z
    200,   // Width
    350 - (100 * (1 + sin(game_state.tick / 100.0f))),  // Height
    16, // Font size
    0.7f, // Font kerning
    0.6f, // Line spacing
    12.0f, // Margin
    some_text,
    UI_TEXT_BUFF_SIZE
  );

  snprintf(some_text, UI_TEXT_BUFF_SIZE, "You have (2) unread messages...");
  render_text(textures[TEXTURE_FONT],
    window.width - (10.0f + 300), 10.0f, // x, y
    0.9f, // z
    300,   // Width
    80,  // Height
    16, // Font size
    0.7f, // Font kerning
    0.6f, // Line spacing
    12.0f, // Margin
    some_text,
    UI_TEXT_BUFF_SIZE
  );
#else
#endif
}

i32 game_execute(i32 window_width, i32 window_height, u8 fullscreen) {
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
  game_run();
  window_close();
  resources_unload();
  return 0;
}
