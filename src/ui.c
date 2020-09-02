// ui.c

#include "game.h"
#include "renderer_common.h"
#include "renderer.h"
#include "window.h"
#include "ui.h"

#define MAX_UI_ELEMENTS 512
#define UI_ID (__LINE__)
#define ELEMENT_TEXT_BUFFER_SIZE (256)
#define PIXEL_TO_GRID(PX, GRID_SIZE) (i32)(PX / GRID_SIZE)

static i32 x_delta = 0;
static i32 y_delta = 0;

enum Element_type {
  ELEMENT_TEXT,
  ELEMENT_BUTTON,
  ELEMENT_CHECKBOX,
};

struct UI_element {
  u32 id;
  i32 x, y, w, h;
  u16 type;
  u16 font_size;
  vec3 font_color;
  const char* text;

  union {
    struct {
      u8 toggle_value;
    };
  };

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
};

static struct UI_state ui = {0};

static void ui_element_init(struct UI_element* e, u32 id, i32 x, i32 y, i32 w, i32 h, u16 type, u16 font_size, const char* text);
static void ui_button_init(struct UI_element* e);
static void ui_element(struct UI_element* e);

void ui_element_init(struct UI_element* e, u32 id, i32 x, i32 y, i32 w, i32 h, u16 type, u16 font_size, const char* text) {
  assert(e);

  e->id = id;
  e->x = x;
  e->y = y;
  e->w = w;
  e->h = h;
  e->type = type;
  e->font_size = font_size;
  e->font_color = VEC3(1, 1, 1);
  e->text = text;

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

void ui_element(struct UI_element* e) {
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
        break;
      }
      case ELEMENT_CHECKBOX: {
        e->toggle_value = !e->toggle_value;
        break;
      }
    }
  }
}

void ui_init() {
  ui.element_count = 0;
}

void ui_update() {
  Game_state* game = &game_state;

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
}

u8 ui_do_button(u32 id, i32 x, i32 y, i32 w, i32 h, const char* text, u16 font_size) {
  struct UI_element* e = NULL;

  if (id >= ui.element_count) {
    e = &ui.elements[id];
    ui_element_init(e, id, x, y, w, h, ELEMENT_BUTTON, font_size, text);
    ui_button_init(e);
    ui.element_count++;
  }
  else {
    e = &ui.elements[id];
  }
  assert(e);
  e->text = text;
  ui_element(e);
  return e->pressed;
}

u8 ui_do_checkbox(u32 id, i32 x, i32 y, i32 w, i32 h, u8 toggle_value, const char* text, u16 font_size) {
  struct UI_element* e = NULL;

  if (id >= ui.element_count) {
    e = &ui.elements[id];
    ui_element_init(e, id, x, y, w, h, ELEMENT_CHECKBOX, font_size, text);
    e->movable = 0;
    ui.element_count++;
  }
  else {
    e = &ui.elements[id];
  }
  assert(e);
  e->text = text;
  ui_element(e);
  if (e->pressed) {
    toggle_value = !toggle_value;
    e->toggle_value = toggle_value;
  }
  return toggle_value;
}

void ui_render() {
  for (u32 i = 0; i < ui.element_count; i++) {
    struct UI_element* e = &ui.elements[i];
    float z_index = 0.8f + (0.1f / (1 + i));
    if (e->text) {
      render_simple_text(textures[TEXTURE_FONT], e->x, e->y, z_index + 0.01f, e->w, e->h, e->font_size, 0.7f, 0.7f, 10.0f, e->text, ELEMENT_TEXT_BUFFER_SIZE);
    }
    if (e->pressed_down) {
      render_rect(e->x, e->y, z_index, e->w, e->h, 1, 1, 1, 1, 0, 1.0f / e->w);
    }
    else if (e->hover) {
      render_rect(e->x, e->y, z_index, e->w, e->h, 0.1f, 0.4f, 0.9f, 1.0f, 0, 1.0f / e->w);
    }
    else {
      render_rect(e->x, e->y, z_index, e->w, e->h, 0.3f, 0.05f, 0.3f, 1.0f, 0, 1.0f / e->w);
    }
    switch (e->type) {
      case ELEMENT_TEXT: {
        break;
      }
      case ELEMENT_BUTTON: {
        break;
      }
      case ELEMENT_CHECKBOX: {
        if (e->toggle_value) {
          render_filled_rect(e->x, e->y, z_index, e->w, e->h, 1, 0, 0, 1, 0);
        }
        break;
      }
    }

    render_filled_rect(e->x, e->y, z_index, e->w, e->h, 0, 0, 0, 1, 0);
  }
}
