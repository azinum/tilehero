// ui.h

#ifndef _UI_H
#define _UI_H

#define UI_ID (__LINE__)

enum UI_context {
  UI_DEFAULT = 0,
  UI_MAIN_MENU,
};

void ui_init();

void ui_focus(u8 id);

void ui_update();

u8 ui_do_button(u32 id, i32 x, i32 y, i32 w, i32 h, const char* text, u16 font_size);

u8 ui_do_checkbox(u32 id, i32 x, i32 y, i32 w, i32 h, u8 toggle_value, const char* text, u16 font_size);

u8 ui_do_text(u32 id, i32 x, i32 y, i32 w, i32 h, const char* text, u16 font_size);

void ui_render();

#endif
