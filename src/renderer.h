// renderer.h

#ifndef _RENDERER_H
#define _RENDERER_H

void renderer_init();

void sprite_init_data(u32* quad_vao);

void render_sprite(u32 program, i32 texture, i32 x, i32 y, vec2 size, float angle, vec3 color, u32 quad_vao);

#endif
