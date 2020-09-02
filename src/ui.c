// ui.c

#include "game.h"
#include "renderer_common.h"
#include "renderer.h"
#include "window.h"
#include "ui.h"

#define MAX_UI_ELEMENTS 64
#define ELEMENT_TEXT_BUFFER_SIZE (256)
#define PIXEL_TO_GRID(PX, GRID_SIZE) (i32)(PX / GRID_SIZE)

typedef void (*event_func)(void*);

enum Element_type {
  ELEMENT_TEXT,
  ELEMENT_BUTTON,
};

struct UI_element {
  i32 x, y, w, h;
  u16 type;
  u16 font_size;
  vec3 font_color;
  char text[ELEMENT_TEXT_BUFFER_SIZE];

  union {
    struct {
      i32 value;
    };
  };

  event_func event;

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

static struct UI_element* add_ui_element(i32 x, i32 y, i32 w, i32 h, u16 type, const char* text, event_func event);
static void test_event(void* p);

static struct UI_element* add_ui_element(i32 x, i32 y, i32 w, i32 h, u16 type, const char* text, event_func event) {
  if (ui.element_count >= MAX_UI_ELEMENTS) {
    return NULL;
  }
  struct UI_element* e = &ui.elements[ui.element_count++];
  e->x = x;
  e->y = y;
  e->w = w;
  e->h = h;
  e->type = type;
  e->font_size = 14;
  e->font_color = VEC3(1, 1, 1);
  strncpy(e->text, text, ELEMENT_TEXT_BUFFER_SIZE);

  e->event = event;

  e->movable = 1;
  e->hover = 0;
  e->pressed = 0;
  e->pressed_down = 0;
  e->snap_to_grid = 1;
  e->grid_size = 16;
  return e;
}

void test_event(void* p) {
  (void)p;
  printf("Event!\n");
}

void ui_init() {
  ui.element_count = 0;

{
  struct UI_element* e = add_ui_element(140, 10, 200, 200, ELEMENT_TEXT, "Hello this is a string! This ui element has some text that we need to display. We can resize this ui element too!", NULL);
  (void)e;
}
{
  struct UI_element* e = add_ui_element(16, 16, 16 * 5, 36, ELEMENT_TEXT, "Button", test_event);
  (void)e;
  e->movable = 0;
  // e->snap_to_grid = 0;
}
}

static i32 x_delta = 0;
static i32 y_delta = 0;

void ui_update() {
  for (u32 i = 0; i < ui.element_count; i++) {
    struct UI_element* e = &ui.elements[i];
    if (mouse_over(window.mouse_x, window.mouse_y, e->x, e->y, e->w, e->h)) {
      e->hover = 1;
    }
    else {
      e->hover = 0;
    }

    if (e->hover) {
      if (left_mouse_pressed | right_mouse_pressed) {
        e->pressed = 1;
        if (e->event) {
          e->event(NULL);
        }

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
  }
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
      render_rect(e->x, e->y, z_index, e->w, e->h, 0.2f, 0.05f, 0.2f, 1.0f, 0, 1.0f / e->w);
    }
    render_filled_rect(e->x, e->y, z_index, e->w, e->h, 0, 0, 0, 1, 0);
  }
}
