// renderer.c

#include "common.h"
#include "renderer_common.h"
#include "shader.h"
#include "resource.h"
#include "window.h"
#include "math_util.h"
#include "renderer.h"

mat4 model, view, projection;

static v4 tint = (v4) {1, 1, 1, 1};
static u32 quad_vao = 0;
static u32 rect_shader,
  filled_rect_shader,
  text_shader;

u32 tile_shader;

struct Texture_program {
  u32 handle;
  // Uniform locations:
  u32 projection;
  u32 view;
  u32 model;

  u32 uv_offset;
  u32 uv_range;
  u32 tint;
};

static float quad_vertices[] = {
  // vertex,  uv coord
  0.0f, 1.0f, 0.0f, 1.0f,
  1.0f, 0.0f, 1.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 0.0f,

  0.0f, 1.0f, 0.0f, 1.0f,
  1.0f, 1.0f, 1.0f, 1.0f,
  1.0f, 0.0f, 1.0f, 0.0f,
};

static struct Texture_program texture_program;

static void init_quad_data();
static void load_texture_shader(struct Texture_program* program, const char* path);

void init_quad_data() {
  u32 vbo;  // TODO(lucas): Delete this later?

  glGenVertexArrays(1, &quad_vao);
  glGenBuffers(1, &vbo);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

  glBindVertexArray(quad_vao);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void load_texture_shader(struct Texture_program* program, const char* path) {
  program->handle = shader_compile(path);

  program->projection = glGetUniformLocation(program->handle, "projection");
  program->view = glGetUniformLocation(program->handle, "view");
  program->model = glGetUniformLocation(program->handle, "model");

  program->uv_offset = glGetUniformLocation(program->handle, "uv_offset");
  program->uv_range = glGetUniformLocation(program->handle, "uv_range");
  program->tint = glGetUniformLocation(program->handle, "tint");
}

void renderer_init() {
  init_quad_data();
  model = MAT4(1.0f);
  view = MAT4(1.0f);
  projection = mm_orthographic(0, window.width, window.height, 0, -1, 1);
  load_texture_shader(&texture_program, "resource/shader/texture");
  rect_shader = shader_compile("resource/shader/rect");
  filled_rect_shader = shader_compile("resource/shader/filled_rect");
  text_shader = shader_compile("resource/shader/text");
  tile_shader = shader_compile("resource/shader/tile");
}

void render_texture_region(struct Texture texture, i32 x, i32 y, float z, i32 w, i32 h, float angle, i32 x_offset, i32 y_offset, i32 x_range, i32 y_range) {
  const struct Texture_program* program = &texture_program;
  glUseProgram(program->handle);

  model = mm_translate(V3(x, y, z));

  translate2d(model, 0.5f * w, 0.5f * h);
  rotate2d(model, angle);
  translate2d(model, -0.5f * w, -0.5f * h);
  scale2d(model, w, h);

  glUniformMatrix4fv(program->projection, 1, GL_FALSE, (float*)&projection);
  glUniformMatrix4fv(program->view, 1, GL_FALSE, (float*)&view);
  glUniformMatrix4fv(program->model, 1, GL_FALSE, (float*)&model);

  glUniform2f(program->uv_offset, (float)x_offset / texture.w, (float)y_offset / texture.h);
  glUniform2f(program->uv_range, (float)x_range / texture.w, (float)y_range / texture.h);
  glUniform4f(program->tint, tint.x, tint.y, tint.z, tint.w);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture.id);

  glBindVertexArray(quad_vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}

void render_sprite(i32 spritesheet_id, i32 sprite_id, i32 x, i32 y, float z, i32 w, i32 h) {
  struct Spritesheet sheet = spritesheets[spritesheet_id];
  i32 x_offset = SHEET_GET_X_OFFSET(sheet, sprite_id);
  i32 y_offset = SHEET_GET_Y_OFFSET(sheet, sprite_id);
  render_texture_region(sheet.texture, x, y, z, w, h, 0 /* angle */, x_offset, y_offset, sheet.w, sheet.h);
}

void render_text(struct Texture font_texture, float x, float y, float z, float w, float h, v3 tint, float size, float kerning, float line_spacing, float margin, const char* text, u32 text_length, u8 background, float rect_r, float rect_g, float rect_b, float rect_a, float border_r, float border_g, float border_b, float border_a, float border_thickness) {

  if (background) {
    render_filled_rectangle(x, y, z - 0.005f, w, h,
      rect_r, rect_g, rect_b, rect_a,
      border_r, border_g, border_b, border_a,
      0, border_thickness / w);
  }

  float font_size = font_texture.w;

  const u32 program = text_shader;
  glUseProgram(program);

  glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, (float*)&projection);
  glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, (float*)&view);

  glUniform3f(glGetUniformLocation(program, "tint"), tint.x, tint.y, tint.z);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, font_texture.id);

  glBindVertexArray(quad_vao);

  float x_position = x + margin;
  float y_position = y + margin;
  for (u32 text_index = 0;
      text_index < text_length &&
      text_index < strlen(text);
      text_index++) {

    char current_char = text[text_index];
    if (current_char == '\0')
      break;
    if (y_position >= (y + h - (margin + (size * line_spacing))))
      break;
    if (x_position >= (x + w - (margin + (size * kerning))))
      break;

    if (current_char >= 32 && current_char < 127 && current_char != '\n') {
      float x_offset = 0;
      float y_offset = (current_char - 32) * font_size;
      float x_range = font_size;
      float y_range = font_size;

      model = mm_translate(V3(x_position, y_position, z));

      scale2d(model, size, size);

      glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, (float*)&model);
      glUniform2f(glGetUniformLocation(program, "uv_offset"), (float)x_offset / font_texture.w, (float)y_offset / font_texture.h);
      glUniform2f(glGetUniformLocation(program, "uv_range"), (float)x_range / font_texture.w, (float)y_range / font_texture.h);

      glDrawArrays(GL_TRIANGLES, 0, 6);

      x_position += size * kerning;
    }
    if ((x_position + (size * kerning)) > (x + w - margin) || current_char == '\n') {
      x_position = x + margin;
      y_position += (size * (2 * line_spacing));
    }
  }

  glBindVertexArray(0);
}

void render_rect(i32 x, i32 y, float z, i32 w, i32 h, float r, float g, float b, float a, float angle, float thickness) {
  const u32 program = rect_shader;
  glUseProgram(program);

  model = mm_translate(V3(x, y, z));

  translate2d(model, 0.5f * w, 0.5f * h);
  rotate2d(model, angle);
  translate2d(model, -0.5f * w, -0.5f * h);

  scale2d(model, w, h);

  glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, (float*)&projection);
  glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, (float*)&view);
  glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, (float*)&model);

  glUniform4f(glGetUniformLocation(program, "in_color"), r, g, b, a);
  glUniform1f(glGetUniformLocation(program, "thickness"), thickness);
  glUniform2f(glGetUniformLocation(program, "rect_size"), w, h);

  glBindVertexArray(quad_vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}

void render_filled_rectangle(i32 x, i32 y, float z, i32 w, i32 h, float r, float g, float b, float a, float border_r, float border_g, float border_b, float border_a, float angle, float thickness) {
  const u32 program = filled_rect_shader;
  glUseProgram(program);

  model = mm_translate(V3(x, y, z));

  translate2d(model, 0.5f * w, 0.5f * h);
  rotate2d(model, angle);
  translate2d(model, -0.5f * w, -0.5f * h);

  scale2d(model, w, h);

  glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, (float*)&projection);
  glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, (float*)&view);
  glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, (float*)&model);

  glUniform4f(glGetUniformLocation(program, "in_color"), r, g, b, a);
  glUniform4f(glGetUniformLocation(program, "border_color"), border_r, border_g, border_b, border_a);
  glUniform1f(glGetUniformLocation(program, "thickness"), thickness);
  glUniform2f(glGetUniformLocation(program, "rect_size"), w, h);

  glBindVertexArray(quad_vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}

void render_instanced_list(struct Instanced_list* l) {
  glBindBuffer(GL_ARRAY_BUFFER, l->instance_buffer);
  glBufferData(GL_ARRAY_BUFFER, l->max_instances * l->data_length * sizeof(float), l->instance_data, GL_STREAM_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  const u32 program = tile_shader;
  glUseProgram(program);

  glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, (float*)&projection);
  glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, (float*)&view);

  glActiveTexture(GL_TEXTURE0);
  if (l->texture) {
    glBindTexture(GL_TEXTURE_2D, l->texture->id);
  }

  glBindVertexArray(l->vao);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, quad_vao);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

  glEnableVertexAttribArray(1);
  glDrawArraysInstanced(GL_TRIANGLES, 0, 6, l->instance_data_count);
  glDisableVertexAttribArray(1);

  glDisableVertexAttribArray(0);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void renderer_set_tint(float r, float g, float b, float a) {
  tint = V4(r, g, b, a);
}

void instanced_list_init(struct Instanced_list* l, u8 data_length, u16 max_instances, struct Texture* texture) {
  l->texture = texture;
  l->max_instances = max_instances;
  l->data_length = data_length;

  glGenVertexArrays(1, &l->vao);
  glBindVertexArray(l->vao);

  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);

  glGenBuffers(1, &l->instance_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, l->instance_buffer);
  glBufferData(GL_ARRAY_BUFFER, data_length * max_instances * sizeof(float), l->instance_data, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glBindVertexArray(l->vao);
  glBindBuffer(GL_ARRAY_BUFFER, l->instance_buffer);

  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, l->instance_data_count * sizeof(float), (void*)(0 * sizeof(float)));
  glVertexAttribDivisor(1, 1);
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

}

void renderer_free() {
  shader_delete(texture_program.handle);
  shader_delete(rect_shader);
  shader_delete(filled_rect_shader);
  shader_delete(text_shader);
  shader_delete(tile_shader);
  glDeleteVertexArrays(1, &quad_vao);
}
