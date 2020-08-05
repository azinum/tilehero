// renderer.c

#include "common.h"
#include "renderer_common.h"
#include "matrix_math.h"
#include "shader.h"
#include "resource.h"
#include "window.h"
#include "renderer.h"

mat4 model, view, projection;

static vec4 tint = (vec4) {1, 1, 1, 1};
static u32 quad_vao = 0;
static u32 sprite_shader, rect_shader;

static void init_quad_data();

void init_quad_data() {
  float vertices[] = {
    // vertex,  uv coord
    0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,

    0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f,
  };
  u32 vbo;  // TODO(lucas): Delete this later?

  glGenVertexArrays(1, &quad_vao);
  glGenBuffers(1, &vbo);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindVertexArray(quad_vao);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void renderer_init() {
  init_quad_data();
  model = mm_mat4d(1.0f);
  view = mm_mat4d(1.0f);
  projection = mm_orthographic(0, window.width, window.height, 0, -1, 1);
  sprite_shader = shader_compile("resource/shader/sprite");
  rect_shader = shader_compile("resource/shader/rect");
}

void render_texture_region(struct Texture texture, float x, float y, float z, float w, float h, float angle, i32 x_offset, i32 y_offset, i32 x_range, i32 y_range) {
  const u32 program = sprite_shader;
  glUseProgram(sprite_shader);

  model = mm_translate((vec3) {x, y, z});

  translate(model, 0.5f * w, 0.5f * h);
  rotate(model, angle);
  translate(model, -0.5f * w, -0.5f * h);
  scale(model, w, h);

  glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, (float*)&projection);
  glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, (float*)&view);
  glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, (float*)&model);

  glUniform2f(glGetUniformLocation(program, "uv_offset"), (float)x_offset / texture.w, (float)y_offset / texture.h);
  glUniform2f(glGetUniformLocation(program, "uv_range"), (float)x_range / texture.w, (float)y_range / texture.h);
  glUniform4f(glGetUniformLocation(program, "tint"), tint.x, tint.y, tint.z, tint.w);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture.id);

  glBindVertexArray(quad_vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}

void render_rect(float x, float y, float z, float w, float h, float r, float g, float b, float angle, float border_width) {
  const u32 program = rect_shader;
  glUseProgram(program);

  model = mm_translate((vec3) {x, y, z});

  translate(model, 0.5f * w, 0.5f * h);
  rotate(model, angle);
  translate(model, -0.5f * w, -0.5f * h);

  scale(model, w, h);

  glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, (float*)&projection);
  glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, (float*)&view);
  glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, (float*)&model);

  glUniform3f(glGetUniformLocation(program, "in_color"), r, g, b);
  glUniform1f(glGetUniformLocation(program, "border_width"), border_width);

  glBindVertexArray(quad_vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}

void renderer_set_tint(float r, float g, float b, float a) {
  tint = (vec4) {
    .x = r,
    .y = g,
    .z = b,
    .w = a,
  };
}

void renderer_free() {
  shader_delete(sprite_shader);
  shader_delete(rect_shader);
  glDeleteVertexArrays(1, &quad_vao);
}
