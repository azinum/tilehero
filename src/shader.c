// shader.c

#include "game_common.h"
#include "renderer_common.h"
#include "file_loader.h"
#include "shader.h"

#define ERR_BUFFER_SIZE 512

// NOTE(lucas): Returns the shader program id/pointer.
i32 shader_compile(const char* shader) {
  i32 shader_program = -1;
  u32 vert_shader = 0;
  u32 frag_shader = 0;
  char path[PATH_LENGTH_MAX] = {0};
  snprintf(path, PATH_LENGTH_MAX, "%s.vert", shader);
  const char* vert_source = read_entire_file(path);
  snprintf(path, PATH_LENGTH_MAX, "%s.frag", shader);
  const char* frag_source = read_entire_file(path);
  if (!vert_source || !frag_source)
    goto done;

  i32 success = 0;
  char err_log[ERR_BUFFER_SIZE];

  vert_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vert_shader, 1, &vert_source, NULL);
  glCompileShader(vert_shader);
{
  glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vert_shader, ERR_BUFFER_SIZE, NULL, err_log);
    fprintf(stderr, "%s\n", err_log);
    goto done;
  }
}

  frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(frag_shader, 1, &frag_source, NULL);
  glCompileShader(frag_shader);
{
  glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(frag_shader, ERR_BUFFER_SIZE, NULL, err_log);
    fprintf(stderr, "%s\n", err_log);
    goto done;
  }
}
  
  shader_program = glCreateProgram();
  glAttachShader(shader_program, vert_shader);
  glAttachShader(shader_program, frag_shader);
  glLinkProgram(shader_program);

{
  glGetProgramiv(shader_program, GL_VALIDATE_STATUS, &success);
  if (success != GL_NO_ERROR) {
    glGetProgramInfoLog(shader_program, ERR_BUFFER_SIZE, NULL, err_log);
    fprintf(stderr, "%s\n", err_log);
    goto done;
  }
}
done:
  if (vert_shader > 0)
    glDeleteShader(vert_shader);
  if (frag_shader > 0)
    glDeleteShader(frag_shader);
  if (vert_source)
    free((void*)vert_source);
  if (frag_source)
    free((void*)frag_source);
  return shader_program;
}

void shader_delete(i32 shader_program) {
  if (shader_program >= 0) {
    glDeleteProgram(shader_program);
  }
}
