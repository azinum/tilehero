// renderer.c

#include "renderer_common.h"
#include "game_common.h"
#include "renderer.h"

typedef struct Renderer_state {
  i16* vbo;
  i16 vbo_count;
} Renderer_state;
