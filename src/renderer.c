// renderer.c

#include "game_common.h"
#include "renderer_common.h"
#include "matrix_math.h"
#include "renderer.h"

#define MAX_VBO 32

typedef struct Renderer_state {
  u32 vbo[MAX_VBO];
  u8 vbo_count;
} Renderer_state;

static Renderer_state renderer_state;

static mat4 projection = {0};

void renderer_init() {
  renderer_state.vbo_count = 0;
  projection = mm_orthographic(0, 1920/4, 1080/4, 0, -1, 1);
}

void sprite_init_data(u32* quad_vao) {
  float vertices[] = {
    0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,

    0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f,
  };
  assert(renderer_state.vbo_count < MAX_VBO);
  u32* vbo = &renderer_state.vbo[renderer_state.vbo_count++];

  glGenVertexArrays(1, quad_vao);
  glGenBuffers(1, vbo);

  glBindBuffer(GL_ARRAY_BUFFER, *vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindVertexArray(*quad_vao);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void render_sprite(u32 program, i32 texture, i32 x, i32 y, vec2 size, float angle, vec3 color, u32 quad_vao) {
  (void)texture;

  mat4 model = mm_mat4d(1.0f);
  translate(model, x, y);

  translate(model, 0.5f * size.x, 0.5f * size.y);
  rotate(model, angle);
  translate(model, -0.5f * size.x, -0.5f * size.y);

  scale(model, size.x, size.y);

  glUseProgram(program);
  glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, (float*)&model);
  glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, (float*)&projection);
  glUniform3f(glGetUniformLocation(program, "sprite_color"), color.x, color.y, color.z);

  // glActiveTexture(GL_TEXTURE0);
  // glBindTexture(GL_TEXTURE_2D, texture);

  glBindVertexArray(quad_vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}
