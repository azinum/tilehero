// renderer.h

#ifndef _RENDERER_H
#define _RENDERER_H

#define render_filled_rect(x, y, z, w, h, r, g, b, a, angle) render_filled_rectangle(x, y, z, w, h, r, g, b, a, 0, 0, 0, 0, angle, 0)

struct Texture;

extern union mat4 model, view, projection;

void renderer_init();

void render_texture_region(struct Texture texture, float x, float y, float z, float w, float h, float angle, i32 x_offset, i32 y_offset, i32 x_range, i32 y_range);

void render_text(struct Texture font_texture, float x, float y, float z, float w, float h, float size, float kerning, float margin, const char* text, u32 text_length);

void render_rect(float x, float y, float z, float w, float h, float r, float g, float b, float a, float angle, float thickness);

void render_filled_rectangle(float x, float y, float z, float w, float h, float r, float g, float b, float a, float border_r, float border_g, float border_b, float border_a, float angle, float thickness);

void renderer_set_tint(float r, float g, float b, float a);

#endif
