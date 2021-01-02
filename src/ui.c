// ui.c

#include "game.h"
#include "renderer_common.h"
#include "renderer.h"
#include "window.h"
#include "audio.h"
#include "ui.h"

#define ELEMENT_TEXT_BUFFER_SIZE (256)
#define PIXEL_TO_GRID(PX, GRID_SIZE) (i32)(PX / GRID_SIZE)

#define UI_PLAY_CLICK_SOUND() audio_play_once(SOUND_0F, 0.2f)

static i32 x_delta = 0;
static i32 y_delta = 0;

struct UI_state ui = {0};

static void ui_element_init(struct UI_element* e, u32 id, i32 x, i32 y, i32 w, i32 h, u16 type, u16 font_size, const char* text, Element_data* data);
static void ui_button_init(struct UI_element* e);
static void ui_interaction(struct UI_element* e);
static struct UI_element* ui_init_interactable(u32 id, i32 x, i32 y, i32 w, i32 h, u16 type, u16 font_size, const char* text, Element_data* data, struct UI_element** elem);

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
  e->background_color = V3(0, 0, 0);
  e->background = 1;
  e->border = 1;
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
    ui.is_interacting = 1;  // TODO(lucas): Temporary?
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

struct UI_element* ui_init_interactable(u32 id, i32 x, i32 y, i32 w, i32 h, u16 type, u16 font_size, const char* text, Element_data* data, UI_element** elem) {
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
    if (elem) {
      *elem = e;
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
  ui.is_interacting = 0;
}

void ui_focus(u8 id) {
  ui.element_iter = 0;
  ui.prev_focus_id = ui.focus_id;
  ui.focus_id = id;
  if (ui.prev_focus_id != ui.focus_id) {
    ui.element_count = 0;
  }
}

u8 ui_do_button(u32 id, i32 x, i32 y, i32 w, i32 h, const char* text, u16 font_size, struct UI_element** elem) {
  struct UI_element* e = ui_init_interactable(id, x, y, w, h, ELEMENT_BUTTON, font_size, text, NULL, elem);
  ui_interaction(e);
  return e->pressed;
}

u8 ui_do_checkbox(u32 id, i32 x, i32 y, i32 w, i32 h, u8 toggle_value, const char* text, u16 font_size, struct UI_element** elem) {
  Element_data data = { .toggle_value = toggle_value };
  struct UI_element* e = ui_init_interactable(id, x, y, w, h, ELEMENT_CHECKBOX, font_size, text, &data, elem);
  ui_interaction(e);
  if (e->pressed) {
    toggle_value = !toggle_value;
    e->data.toggle_value = toggle_value;
  }
  return toggle_value;
}

u8 ui_do_text(u32 id, i32 x, i32 y, i32 w, i32 h, const char* text, u16 font_size, struct UI_element** elem) {
  struct UI_element* e = ui_init_interactable(id, x, y, w, h, ELEMENT_TEXT, font_size, text, NULL, elem);
  ui_interaction(e);
  e->text = text;
  return e->pressed;
}

void ui_update() {
  ui.is_interacting = 0;
}

void ui_render() {
  for (u32 i = 0; i < ui.element_count; i++) {
    struct UI_element* e = &ui.elements[i];
    float z_index = 0.8f + (0.1f / (1 + i));
    v3 tint = V3(0, 0, 0);

    if (e->text) {
      render_text(textures[TEXTURE_FONT], e->x, e->y, z_index, e->w, e->h, e->font_color, e->font_size, 0.7f, 0.7f, 10.0f, e->text, ELEMENT_TEXT_BUFFER_SIZE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    }

    if (e->pressed_down) {
      if (e->border) {
        render_rect(e->x, e->y, z_index, e->w, e->h, 1, 1, 1, 1, 0, 2.0f / e->w);
      }
      tint.x = tint.y = tint.z -= 0.13f;
    }
    else if (e->hover) {
      if (e->border) {
        render_rect(e->x, e->y, z_index, e->w, e->h, 0.1f, 0.4f, 0.9f, 1.0f, 0, 2.0f / e->w);
      }
      tint.x = tint.y = tint.z += 0.05f;
    }
    else {
      if (e->border) {
        render_rect(e->x, e->y, z_index, e->w, e->h, 0.3f, 0.05f, 0.3f, 1.0f, 0, 2.0f / e->w);
      }
    }
    if (e->background) {
      render_filled_rect(e->x, e->y, z_index, e->w, e->h, e->background_color.x + tint.x, e->background_color.y + tint.y, e->background_color.z + tint.z, 1, 0);
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
          render_sprite(SHEET_UI, UI_SPRITE_CHECK, e->x, e->y, z_index + 0.01f, e->w, e->h);
        }
        else {
          render_sprite(SHEET_UI, UI_SPRITE_CROSS, e->x, e->y, z_index + 0.01f, e->w, e->h);
        }
        break;
      }
    }
  }
}
