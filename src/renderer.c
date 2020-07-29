// renderer.c

#include "common.h"
#include "renderer_common.h"
#include "matrix_math.h"
#include "shader.h"
#include "renderer.h"

static mat4 projection = {0};
static u32 quad_vao = 0;
static u32 sprite_shader, rect_shader;

void renderer_init() {
  projection = mm_orthographic(0, 1920 / 4, 1080 / 4, 0, -1, 1);
  sprite_shader = shader_compile("resources/shaders/sprite");
  rect_shader = shader_compile("resources/shaders/rect");
}

void sprite_init_data() {
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

void render_sprite(u32 texture, i32 x, i32 y, i32 w, i32 h, float angle) {
  const u32 program = sprite_shader;
  glUseProgram(program);

  mat4 model = mm_mat4d(1.0f);
  translate(model, x, y);

  translate(model, 0.5f * w, 0.5f * h);
  rotate(model, angle);
  translate(model, -0.5f * w, -0.5f * h);

  scale(model, w, h);

  glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, (float*)&model);
  glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, (float*)&projection);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);

  glBindVertexArray(quad_vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}

void render_rect(i32 x, i32 y, i32 w, i32 h, float r, float g, float b, float angle, float border_width) {
  const u32 program = rect_shader;
  glUseProgram(program);

  mat4 model = mm_mat4d(1.0f);
  translate(model, x, y);
  translate(model, 0.5f * w, 0.5f * h);
  rotate(model, angle);
  translate(model, -0.5f * w, -0.5f * h);
  scale(model, w, h);

  glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, (float*)&model);
  glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, (float*)&projection);
  glUniform3f(glGetUniformLocation(program, "in_color"), r, g, b);
  glUniform1f(glGetUniformLocation(program, "border_width"), border_width);

  glBindVertexArray(quad_vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}
