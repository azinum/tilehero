// window.c

#include <assert.h>

#include "common.h"
#include "config.h"
#include "renderer_common.h"
#include "math_util.h"
#include "renderer.h"
#include "shader.h"
#include "window.h"

i8 mouse_state = 0;
i8 key_down[GLFW_KEY_LAST] = {0};
i8 key_pressed[GLFW_KEY_LAST] = {0};

static void opengl_configure();
static void framebuffer_size_callback(GLFWwindow* window, i32 width, i32 height);

void opengl_configure() {
  glEnable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glShadeModel(GL_FLAT);
  glEnable(GL_TEXTURE_2D);
  glAlphaFunc(GL_GREATER, 1);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// NOTE(lucas): This callback function is called when resizing the window.
void framebuffer_size_callback(GLFWwindow* glfw_window, i32 width, i32 height) {
  glViewport(0, 0, width, height);
  glfwGetWindowSize(glfw_window, &width, &height);
  window.width = width;
  window.height = height;
  projection = mm_orthographic(0, width, height, 0, -1, 1);
}

i32 window_open(i32 width, i32 height, u8 fullscreen, const char* title) {
  window.title = title;
  window.width = width;
  window.height = height;
  window.init_width = width;
  window.init_height = height;
  window.fullscreen = fullscreen;
  window.windowed_fullscreen = 0;
  window.mouse_x = 0;
  window.mouse_y = 0;

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_FOCUSED, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
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
  // glfwSetInputMode(window.window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

  i32 glew_err = glewInit();
  if (glew_err != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW: %s\n", glewGetErrorString(glew_err));
    return -1;
  }
  glfwSwapInterval(VSYNC);
  opengl_configure();
  renderer_init();
  return 0;
}

void window_clear() {
  glClearColor(0, 0, 0, 1.0f);
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

i32 window_process_input() {
  glfwGetCursorPos(window.window, &window.mouse_x, &window.mouse_y);
  for (u16 i = 0; i < GLFW_KEY_LAST; i++) {
    i32 key_state = glfwGetKey(window.window, i);
    if (key_state == GLFW_PRESS) {
      key_pressed[i] = !key_down[i];
      key_down[i] = 1;
    }
    else {
      key_down[i] = 0;
      key_pressed[i] = 0;
    }
  }

  i32 left_mouse_button_state = glfwGetMouseButton(window.window, 0);
  i32 right_mouse_button_state = glfwGetMouseButton(window.window, 1);

  (left_mouse_button_state && !(mouse_state & (1 << 7))) ? mouse_state |= (1 << 6) : (mouse_state &= ~(1 << 6));
  left_mouse_button_state ? mouse_state |= (1 << 7) : (mouse_state &= ~(1 << 7));
  (right_mouse_button_state && !(mouse_state & (1 << 5))) ? mouse_state |= (1 << 4) : (mouse_state &= ~(1 << 4));
  right_mouse_button_state ? mouse_state |= (1 << 5) : (mouse_state &= ~(1 << 5));

  if (key_pressed[GLFW_KEY_ESCAPE]) {
    glfwSetWindowShouldClose(window.window, 1);
  }
  if (key_pressed[GLFW_KEY_F11]) {
    window.windowed_fullscreen = !window.windowed_fullscreen;
    if (window.windowed_fullscreen) {
      const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
      window.width = mode->width;
      window.height = mode->height;
    }
    else {
      window.width = window.init_width;
      window.height = window.init_height;
    }
    glfwSetWindowSize(window.window, window.width, window.height);
  }
  return 0;
}

void window_close() {
  if (window.window) {
    glfwDestroyWindow(window.window);
    glfwTerminate();
  }
}
