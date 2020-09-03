// window.h

#ifndef _WINDOW_H
#define _WINDOW_H

struct {
  void* window;
  const char* title;
  i32 width;
  i32 height;
  i32 init_width;
  i32 init_height;
  u8 windowed_fullscreen;
  u8 fullscreen;
  double mouse_x;
  double mouse_y;
} window;

extern i8 mouse_state;
extern i8 key_down[];
extern i8 key_pressed[];

#define left_mouse_down      (mouse_state & (1 << 7))
#define left_mouse_pressed   (mouse_state & (1 << 6))
#define right_mouse_down     (mouse_state & (1 << 5))
#define right_mouse_pressed  (mouse_state & (1 << 4))

#define middle_mouse_down    (mouse_state & (1 << 3))
#define middle_mouse_pressed (mouse_state & (1 << 2))

i32 window_open(i32 width, i32 height, u8 fullscreen, const char* title);

void window_toggle_fullscreen();

void window_clear();

void window_swapbuffers();

u8 window_should_close();

i32 window_pollevents();

i32 window_process_input();

void window_close();

#endif
