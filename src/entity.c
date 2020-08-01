// entity.c

#include "common.h"
#include "renderer.h"
#include "entity.h"

void entity_init(struct Entity* e, float x, float y, float w, float h) {
  memset(e, 0, sizeof(Entity));
  e->x = x;
  e->y = y;
  e->w = w;
  e->h = h;
}

void entity_render(struct Entity* e) {
  u32 texture = 1;
  render_rect(e->x, e->y, e->w, e->h, 0.9f, 0.1f, 0.12f, 0, 0.03f);
  render_texture_region(texture, e->x, e->y, e->w, e->h, 0, 0, 0, 8, 8, 48, 8);
}
