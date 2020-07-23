// window.c

#include <assert.h>

#include "game_common.h"
#include "renderer_common.h"
#include "matrix_math.h"
#include "renderer.h"
#include "shader.h"
#include "window.h"

struct {
  void* window;
  const char* title;
  i32 width;
  i32 height;
  u8 fullscreen;
} window_state;

static void opengl_configure();
static void framebuffer_size_callback(GLFWwindow* window, i32 width, i32 height);
static void enter_fullscreen();

void opengl_configure() {
  glEnable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_FLAT);
}

// NOTE(lucas): This callback function is called when resizing the window.
void framebuffer_size_callback(GLFWwindow* window, i32 width, i32 height) {
  (void)window;
  glViewport(0, 0, width, height);
}

void enter_fullscreen() {
  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  window_state.window = glfwCreateWindow(
    glfwGetVideoMode(monitor)->width,
    glfwGetVideoMode(monitor)->height,
    window_state.title,
    monitor,
    NULL
  );
}

// TODO(lucas): Get rid of this!
i32 sprite_shader = 0;
u32 quad_vao;

i32 window_open(i32 width, i32 height, u8 fullscreen, const char* title) {
  window_state.title = title;
  window_state.width = width;
  window_state.height = height;
  window_state.fullscreen = fullscreen;

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_FLOATING, GL_TRUE);
#if defined(__APPLE__)
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  if (fullscreen) {
    enter_fullscreen();
  }
  else {
    window_state.window = glfwCreateWindow(width, height, title, NULL, NULL);
  }
  if (!window_state.window) {
    fprintf(stderr, "Failed to create window\n");
    return -1;
  }
  glfwMakeContextCurrent(window_state.window);
  glfwSetFramebufferSizeCallback(window_state.window, framebuffer_size_callback);

  i32 glew_err = glewInit();
  if (glew_err != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW: %s\n", glewGetErrorString(glew_err));
    return -1;
  }
  glfwSwapInterval(1);
  opengl_configure();
  sprite_shader = shader_compile("resources/shaders/sprite");
  sprite_init_data(&quad_vao);
  renderer_init();
  return 0;
}

void window_clear() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

float x = 0;
float y = 0;

void window_swapbuffers() {
  render_sprite(sprite_shader, -1, x, y, (vec2) {10, 10}, 0, (vec3) {0.9f, 0.1f, 0.12f}, quad_vao);
  glfwSwapBuffers(window_state.window);
}

u8 window_should_close() {
  return glfwWindowShouldClose(window_state.window);
}

i32 window_pollevents() {
  glfwPollEvents();
  return 0;
}

i32 window_process_input() {
  if (glfwGetKey(window_state.window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window_state.window, 1);
  }
  if (glfwGetKey(window_state.window, GLFW_KEY_A) == GLFW_PRESS) {
    x -= 1.0f;
  }
  if (glfwGetKey(window_state.window, GLFW_KEY_D) == GLFW_PRESS) {
    x += 1.0f;
  }
  if (glfwGetKey(window_state.window, GLFW_KEY_W) == GLFW_PRESS) {
    y -= 1.0f;
  }
  if (glfwGetKey(window_state.window, GLFW_KEY_S) == GLFW_PRESS) {
    y += 1.0f;
  }
  return 0;
}

void window_close() {
  if (window_state.window) {
    glfwDestroyWindow(window_state.window);
  }
  glfwTerminate();
}
