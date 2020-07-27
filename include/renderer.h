// renderer.h

#ifndef _RENDERER_H
#define _RENDERER_H

void renderer_init();

void sprite_init_data();

void render_sprite(u32 program, u32 texture, i32 x, i32 y, i32 w, i32 h, float angle, vec3 color);

#endif
