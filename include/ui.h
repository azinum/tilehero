// ui.h

#ifndef _UI_H
#define _UI_H

#define UI_ID (__LINE__)
#define UI_INIT(ELEM, BODY) { \
  if (ELEM) { \
    BODY \
    ui_element_post_init(ELEM); \
    ELEM = NULL; \
  } \
}

#define UI_INIT_DEFAULT(ELEM, CALLBACK) { \
  if (ELEM) { \
    CALLBACK(ELEM); \
    ui_element_post_init(ELEM); \
    ELEM = NULL; \
  } \
}

#define VW(VALUE) ((VALUE / 100.0f) * window.width)
#define VH(VALUE) ((VALUE / 100.0f) * window.height)

#define UI_TEXT_BUFFER_SIZE 512
#define MAX_UI_ELEMENTS 256

enum UI_context {
  UI_NONE = 0,
  UI_DEFAULT,
  UI_MAIN_MENU,
  UI_EDITOR,
};

enum Element_type {
  ELEMENT_TEXT,
  ELEMENT_BUTTON,
  ELEMENT_CHECKBOX,
};

enum Element_alignment {
  ALIGN_LEFT = 0, // Default
  ALIGN_RIGHT,
};

typedef union Element_data {
  struct {
    u8 toggle_value;
  };
} Element_data;

typedef struct UI_element {
  u32 id;
  i32 x, y, w, h;
  u16 type;
  u16 font_size;
  v3 font_color;
  v3 background_color;
  u8 background;
  u8 border;
  const char* text;

  Element_data data;

  u8 align; // Unused
  u8 movable;
  u8 hover;
  u8 pressed;
  u8 pressed_down;
  u8 snap_to_grid;
  u16 grid_size;
} UI_element;

struct UI_state {
  struct UI_element elements[MAX_UI_ELEMENTS];
  u32 element_count;
  u32 element_iter;
  u8 focus_id;
  u8 prev_focus_id;
  u8 is_interacting;
};

extern struct UI_state ui;

void ui_init();

void ui_element_post_init(struct UI_element* e);

void ui_focus(u8 id);

void ui_update();

u8 ui_do_button(u32 id, i32 x, i32 y, i32 w, i32 h, const char* text, u16 font_size, struct UI_element** elem);

u8 ui_do_checkbox(u32 id, i32 x, i32 y, i32 w, i32 h, u8 toggle_value, const char* text, u16 font_size, struct UI_element** elem);

u8 ui_do_text(u32 id, i32 x, i32 y, i32 w, i32 h, const char* text, u16 font_size, struct UI_element** elem);

void ui_update();

void ui_render();

#endif
