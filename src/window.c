// window.c

#include <assert.h>

#include "common.h"
#include "renderer_common.h"
#include "matrix_math.h"
#include "renderer.h"
#include "shader.h"
#include "texture.h"
#include "camera.h" // NOTE(lucas): TEMP
#include "window.h"

static void opengl_configure();
static void framebuffer_size_callback(GLFWwindow* window, i32 width, i32 height);

void opengl_configure() {
  glEnable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_FLAT);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// NOTE(lucas): This callback function is called when resizing the window.
void framebuffer_size_callback(GLFWwindow* glfw_window, i32 width, i32 height) {
  (void)glfw_window;
  window.width = width;
  window.height = height;
  glViewport(0, 0, width, height);
  projection = mm_orthographic(0, width, height, 0, -1, 1);
}

// TODO(lucas): Get rid of this!
u32 texture_id = 0;

i32 window_open(i32 width, i32 height, u8 fullscreen, const char* title) {
  window.title = title;
  window.width = width;
  window.height = height;
  window.fullscreen = fullscreen;
  window.mouse_x = 0;
  window.mouse_y = 0;

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_FLOATING, GL_TRUE);
#if defined(__APPLE__)
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  window.window = glfwCreateWindow(width, height, title, fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
  if (!window.window) {
    fprintf(stderr, "Failed to create window\n");
    return -1;
  }
  glfwMakeContextCurrent(window.window);
  glfwSetFramebufferSizeCallback(window.window, framebuffer_size_callback);

  i32 glew_err = glewInit();
  if (glew_err != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW: %s\n", glewGetErrorString(glew_err));
    return -1;
  }
  glfwSwapInterval(1);
  opengl_configure();
  renderer_init();
  texture_id = load_texture("resources/sprites/spritesheet.png");
  return 0;
}

void window_clear() {
  glClearColor(0.05f, 0.1f, 0.2f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void window_swapbuffers() {
  glfwSwapBuffers(window.window);
}

u8 window_should_close() {
  return glfwWindowShouldClose(window.window);
}

i32 window_pollevents() {
  glfwPollEvents();
  return 0;
}

float a = 1;

i32 window_process_input() {
  if (glfwGetKey(window.window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window.window, 1);
  }
  if (glfwGetKey(window.window, GLFW_KEY_A) == GLFW_PRESS) {
    camera.x -= 5.0f;
  }
  if (glfwGetKey(window.window, GLFW_KEY_D) == GLFW_PRESS) {
    camera.x += 5.0f;
  }
  if (glfwGetKey(window.window, GLFW_KEY_W) == GLFW_PRESS) {
    camera.y -= 5.0f;
  }
  if (glfwGetKey(window.window, GLFW_KEY_S) == GLFW_PRESS) {
    camera.y += 5.0f;
  }
  glfwGetCursorPos(window.window, &window.mouse_x, &window.mouse_y);
  return 0;
}

void window_close() {
  if (window.window) {
    glfwDestroyWindow(window.window);
  }
  glfwTerminate();
}
