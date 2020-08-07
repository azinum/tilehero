// renderer.h

#ifndef _RENDERER_H
#define _RENDERER_H

struct Texture;

extern union mat4 model, view, projection;

void renderer_init();

void render_texture_region(struct Texture texture, float x, float y, float z, float w, float h, float angle, i32 x_offset, i32 y_offset, i32 x_range, i32 y_range);

void render_text(struct Texture font_texture, float x, float y, float z, float w, float h, float size, float kerning, const char* text, u32 text_length);

void render_rect(float x, float y, float z, float w, float h, float r, float g, float b, float a, float angle, float thickness);

void renderer_set_tint(float r, float g, float b, float a);

#endif
