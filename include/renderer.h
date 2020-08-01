// renderer.h

#ifndef _RENDERER_H
#define _RENDERER_H

void renderer_init();

void render_sprite(u32 texture, i32 x, i32 y, i32 w, i32 h, float angle);

void render_texture_region(u32 texture, float x, float y, float w, float h, float angle, i32 x_offset, i32 y_offset, i32 x_range, i32 y_range, i32 texture_width, i32 texture_height);

void render_rect(float x, float y, float z, float w, float h, float r, float g, float b, float angle, float border_width);

#endif
