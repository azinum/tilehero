// game.h

#include "common.h"
#include "game_common.h"
#include "util.h"
#include "math_util.h"
#include "config.h"
#include "camera.h"
#include "entity.h"
#include "resource.h"
#include "tile.h"
#include "level.h"
#include "ui.h"

#define TIME_SCALING_MIN (0)
#define TIME_SCALING_MAX (4.0f)

enum Status_code {
  NO_ERR = 0,
  ERR = -1,
};

enum Game_mode {
  MODE_MENU,
  MODE_GAME,
  MODE_PAUSE,
  MODE_EDITOR,
};

typedef struct Game_state {
  World world;
  Save_state save_state;
  float time;
  float total_time;
  float time_scale;
  float delta_time;
  float move_timer;
  u8 should_move;
  u8 is_running;
  u8 mode;
} Game_state;

typedef union Event_arg {
  i32 i;
  float f;
  Entity* entity;
  void* p;
} Event_arg;

typedef void (*event_func)(struct Entity*, Event_arg arg);

typedef struct Event {
  i32 target_type;
  i32 target_flags;
  event_func func;
  Event_arg arg;
} Event;

enum Event_target_type {
  EVENT_TARGET_TYPE_ANY = MAX_ENTITY_TYPE,
};

extern struct Game_state game_state;

void game_entity_remove(struct Entity* e);

Entity* game_copy_add_entity(Entity copy);

Entity* game_add_entity(float x, float y, float w, float h);

Entity* game_add_empty_entity();

Entity* game_add_living_entity(i32 x_tile, i32 y_tile, float w, float h, i8 x_dir, i8 y_dir, i16 health, i16 max_health, i16 attack);

i32 game_load_level(i32 index);

i32 game_load_level_on_complete(i32 index);

void game_fade_to_black();

void game_fade_from_black();

void game_send_message(char* fmt, ...);

i32 game_create_event(i32 type, i32 flags, event_func func, Event_arg arg);

i32 game_execute(i32 window_width, i32 window_height, u8 fullscreen);

void game_restart();
