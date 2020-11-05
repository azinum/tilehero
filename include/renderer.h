// renderer.h

#ifndef _RENDERER_H
#define _RENDERER_H

#include "tile.h" // TMP

// TODO(lucas): Implement instancing!
struct Instanced_list {
  struct Texture* texture;
  u32 vao;
  u32 instance_buffer;
  float instance_data[TILE_COUNT_X * TILE_COUNT_Y * 4];
  u16 instance_data_count;
  u16 max_instances;
  u8 data_length;
};

#define render_filled_rect(x, y, z, w, h, r, g, b, a, angle) \
  render_filled_rectangle(x, y, z, w, h, r, g, b, a, 0, 0, 0, 0, angle, 0)

#define render_simple_text(tex, x, y, z, w, h, size, kerning, line_spacing, margin, text, text_length) \
  render_text(tex, x, y, z, w, h, size, kerning, line_spacing, margin, text, text_length, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)

struct Texture;

extern union mat4 model, view, projection;

extern u32 tile_shader;

void renderer_init();

void render_texture_region(struct Texture texture, i32 x, i32 y, float z, i32 w, i32 h, float angle, i32 x_offset, i32 y_offset, i32 x_range, i32 y_range);

void render_sprite(i32 spritesheet_id, i32 sprite_id, i32 x, i32 y, float z, i32 w, i32 h);

void render_text(struct Texture font_texture, float x, float y, float z, float w, float h, float size, float kerning, float line_spacing, float margin, const char* text, u32 text_length, u8 background, float rect_r, float rect_g, float rect_b, float rect_a, float border_r, float border_g, float border_b, float border_a, float border_thickness);

void render_rect(i32 x, i32 y, float z, i32 w, i32 h, float r, float g, float b, float a, float angle, float thickness);

void render_filled_rectangle(i32 x, i32 y, float z, i32 w, i32 h, float r, float g, float b, float a, float border_r, float border_g, float border_b, float border_a, float angle, float thickness);

void render_instanced_list(struct Instanced_list* l);

void renderer_set_tint(float r, float g, float b, float a);

void instanced_list_init(struct Instanced_list* l, u8 data_length, u16 max_instances, struct Texture* texture);

void renderer_free();

#endif
