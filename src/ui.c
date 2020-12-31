// ui.c

#include "game.h"
#include "renderer_common.h"
#include "renderer.h"
#include "window.h"
#include "audio.h"
#include "ui.h"

#define MAX_UI_ELEMENTS 256
#define UI_TEXT_BUFFER_SIZE 512

#define ELEMENT_TEXT_BUFFER_SIZE (256)
#define PIXEL_TO_GRID(PX, GRID_SIZE) (i32)(PX / GRID_SIZE)

#define UI_PLAY_CLICK_SOUND() audio_play_once(SOUND_0F, 0.2f)

static i32 x_delta = 0;
static i32 y_delta = 0;

enum Element_state {
  ELEM_MOVABLE      = 1 << 0,
  ELEM_HOVER        = 1 << 1,
  ELEM_PRESSED      = 1 << 2,
  ELEM_PRESSED_DOWN = 1 << 3,
};

enum Element_type {
  ELEMENT_TEXT,
  ELEMENT_BUTTON,
  ELEMENT_CHECKBOX,
};

typedef union Element_data {
  struct {
    u8 toggle_value;
  };
} Element_data;

struct UI_element {
  u32 id;
  i32 x, y, w, h;
  u16 type;
  u16 font_size;
  v3 font_color;
  const char* text;

  Element_data data;

  u32 state;

  u8 movable;
  u8 hover;
  u8 pressed;
  u8 pressed_down;
  u8 snap_to_grid;
  u16 grid_size;
};

struct UI_state {
  struct UI_element elements[MAX_UI_ELEMENTS];
  u32 element_count;
  u32 element_iter;
  u8 focus_id;
  u8 prev_focus_id;
};

static struct UI_state ui = {0};

static void ui_element_init(struct UI_element* e, u32 id, i32 x, i32 y, i32 w, i32 h, u16 type, u16 font_size, const char* text, Element_data* data);
static void ui_button_init(struct UI_element* e);
static void ui_interaction(struct UI_element* e);
static struct UI_element* ui_init_interactable(u32 id, i32 x, i32 y, i32 w, i32 h, u16 type, u16 font_size, const char* text, Element_data* data);

void ui_element_init(struct UI_element* e, u32 id, i32 x, i32 y, i32 w, i32 h, u16 type, u16 font_size, const char* text, Element_data* data) {
  assert(e);

  e->id = id;
  e->x = x;
  e->y = y;
  e->w = w;
  e->h = h;
  e->type = type;
  e->font_size = font_size;
  e->font_color = V3(1, 1, 1);
  e->text = text;

  if (data) {
    e->data = *data;
  }

  e->movable = 1;
  e->hover = 0;
  e->pressed = 0;
  e->pressed_down = 0;
  e->snap_to_grid = 1;
  e->grid_size = 16;
}

void ui_button_init(struct UI_element* e) {
  e->movable = 0;
}

void ui_interaction(struct UI_element* e) {
  if (mouse_over(window.mouse_x, window.mouse_y, e->x, e->y, e->w, e->h)) {
    e->hover = 1;
  }
  else {
    e->hover = 0;
  }
  if (e->hover) {
    if (left_mouse_pressed | right_mouse_pressed) {
      e->pressed = 1;
      x_delta = e->x - window.mouse_x;
      y_delta = e->y - window.mouse_y;
    }
    else {
      e->pressed = 0;
    }
    if (left_mouse_down) {
      e->pressed_down = 1;
      if (e->movable) {
        e->x = window.mouse_x + x_delta;
        e->y = window.mouse_y + y_delta;
      }
    }
    else if (right_mouse_down) {
      e->pressed_down = 1;
      if (e->movable) {
        e->w = (window.mouse_x - e->x) + e->grid_size + 6;
        e->h = (window.mouse_y - e->y) + e->grid_size + 6;
      }
    }
    else {
      e->pressed_down = 0;
    }
  }
  else {
    e->pressed = 0;
    e->pressed_down = 0;
  }
  if (e->snap_to_grid) {
    e->x = e->grid_size * PIXEL_TO_GRID(e->x, e->grid_size);
    e->y = e->grid_size * PIXEL_TO_GRID(e->y, e->grid_size);
    e->w = e->grid_size * PIXEL_TO_GRID(e->w, e->grid_size);
    e->h = e->grid_size * PIXEL_TO_GRID(e->h, e->grid_size);
  }
  if (e->pressed) {
    switch (e->type) {
      case ELEMENT_TEXT: {
        break;
      }
      case ELEMENT_BUTTON: {
        UI_PLAY_CLICK_SOUND();
        break;
      }
      case ELEMENT_CHECKBOX: {
        UI_PLAY_CLICK_SOUND();
        e->data.toggle_value = !e->data.toggle_value;
        break;
      }
    }
  }
}

struct UI_element* ui_init_interactable(u32 id, i32 x, i32 y, i32 w, i32 h, u16 type, u16 font_size, const char* text, Element_data* data) {
  struct UI_element* e = NULL;

  if (ui.element_iter >= ui.element_count) {
    e = &ui.elements[ui.element_iter];
    ui_element_init(e, id, x, y, w, h, type, font_size, text, data);
    ui.element_count++;
    e->movable = 0;
    switch (e->type) {
      case ELEMENT_BUTTON:
        ui_button_init(e);
        break;
      case ELEMENT_TEXT:
        e->movable = 1;
        e->snap_to_grid = 1;
        break;
    }
  }
  else {
    e = &ui.elements[ui.element_iter];
  }
  ui.element_iter++;
  e->text = text;
  assert(e);
  return e;
}

void ui_init() {
  ui.element_count = 0;
  ui.element_iter = 0;
}

void ui_focus(u8 id) {
  ui.element_iter = 0;
  ui.prev_focus_id = ui.focus_id;
  ui.focus_id = id;
  if (ui.prev_focus_id != ui.focus_id) {
    ui.element_count = 0;
  }
}

void ui_update() {
  Game_state* game = &game_state;

  ui_focus(UI_DEFAULT);
  if (ui_do_button(0, 16, 16, 16 * 6, 16 * 2, "Restart", 14)) {
    game_restart();
  }
  if (ui_do_button(1, 16, 16 * 4, 16 * 8, 16 * 2, "Next level", 14)) {
    game_load_level(game->level.index + 1);
  }
  if (ui_do_button(2, 16, 16 * 7, 16 * 8, 16 * 2, "Prev level", 14)) {
    if (game->level.index > 0) {
      game_load_level(game->level.index - 1);
    }
  }

  audio_engine.muted = ui_do_checkbox(3, 16, 16 * 10, 32, 32, audio_engine.muted, NULL, 0);
  camera.has_target = ui_do_checkbox(4, 16, 16 * 13, 32, 32, camera.has_target, NULL, 0);

  static char ui_text[UI_TEXT_BUFFER_SIZE] = {0};
    snprintf(ui_text, UI_TEXT_BUFFER_SIZE,
      "entity count: %i/%i\n"
      "fps: %i\n"
      "time: %.3f\n"
      "time scale: %i %%\n"
      "level: %i\n"
      ,
      game->level.entity_count, MAX_ENTITY,
      (i32)(1.0f / game->delta_time),
      game->time,
      (i32)(100 * game->time_scale),
      game->level.index
    );
  ui_do_text(5, 16 * 1, window.height - (16 * 11), 16 * 16, 16 * 9, ui_text, 14);
}


u8 ui_do_button(u32 id, i32 x, i32 y, i32 w, i32 h, const char* text, u16 font_size) {
  struct UI_element* e = ui_init_interactable(id, x, y, w, h, ELEMENT_BUTTON, font_size, text, NULL);
  ui_interaction(e);
  return e->pressed;
}

u8 ui_do_checkbox(u32 id, i32 x, i32 y, i32 w, i32 h, u8 toggle_value, const char* text, u16 font_size) {
  Element_data data = { .toggle_value = toggle_value };
  struct UI_element* e = ui_init_interactable(id, x, y, w, h, ELEMENT_CHECKBOX, font_size, text, &data);
  ui_interaction(e);
  if (e->pressed) {
    toggle_value = !toggle_value;
    e->data.toggle_value = toggle_value;
  }
  return toggle_value;
}

u8 ui_do_text(u32 id, i32 x, i32 y, i32 w, i32 h, const char* text, u16 font_size) {
  struct UI_element* e = ui_init_interactable(id, x, y, w, h, ELEMENT_TEXT, font_size, text, NULL);
  ui_interaction(e);
  e->text = text;
  return e->pressed;
}

void ui_render() {
  for (u32 i = 0; i < ui.element_count; i++) {
    struct UI_element* e = &ui.elements[i];
    float z_index = 0.8f + (0.1f / (1 + i));
    if (e->text) {
      render_simple_text(textures[TEXTURE_FONT], e->x, e->y, z_index, e->w, e->h, e->font_size, 0.7f, 0.7f, 10.0f, e->text, ELEMENT_TEXT_BUFFER_SIZE);
    }
    if (e->pressed_down) {
      render_rect(e->x, e->y, z_index, e->w, e->h, 1, 1, 1, 1, 0, 2.0f / e->w);
    }
    else if (e->hover) {
      render_rect(e->x, e->y, z_index, e->w, e->h, 0.1f, 0.4f, 0.9f, 1.0f, 0, 2.0f / e->w);
    }
    else {
      render_rect(e->x, e->y, z_index, e->w, e->h, 0.3f, 0.05f, 0.3f, 1.0f, 0, 2.0f / e->w);
    }
    switch (e->type) {
      case ELEMENT_TEXT: {
        break;
      }
      case ELEMENT_BUTTON: {
        break;
      }
      case ELEMENT_CHECKBOX: {
        if (e->data.toggle_value) {
          render_sprite(SHEET_UI, UI_SPRITE_CHECK, e->x, e->y, z_index, e->w, e->h);
        }
        else {
          render_sprite(SHEET_UI, UI_SPRITE_CROSS, e->x, e->y, z_index, e->w, e->h);
        }
        break;
      }
    }

    render_filled_rect(e->x, e->y, z_index, e->w, e->h, 0.5, 0.5, 0.5, 1, 0);
  }
}
