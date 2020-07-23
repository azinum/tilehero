// window.h

#ifndef _WINDOW_H
#define _WINDOW_H

i32 window_open(i32 width, i32 height, u8 fullscreen, const char* title);

void window_clear();

void window_swapbuffers();

u8 window_should_close();

i32 window_pollevents();

i32 window_process_input();

void window_close();

#endif
