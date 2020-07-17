// sprite_renderer.h

#ifndef _SPRITE_RENDERER_H
#define _SPRITE_RENDERER_H

#include "matrix_math.h"

void sprite_init_data(u32* vbo, u32* quad_vao);

void render_sprite(u32 program, i32 texture, i32 x, i32 y, vec3 color, u32 quad_vao);

#endif
