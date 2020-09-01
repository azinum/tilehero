// main.c
// tile hero 2

#include "game.h"

#include "window.h"
#include "renderer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

int main(void) {
  window_open(800, 600, 0, "Tile Hero");
  window_close();
  // game_execute(1400, 900, 1);
  // game_execute(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_FULLSCREEN);
  return 0;
}
