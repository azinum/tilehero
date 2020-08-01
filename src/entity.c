// entity.c

#include "common.h"
#include "renderer.h"
#include "game.h"
#include "entity.h"

void entity_init(Entity* e, float x, float y, float w, float h) {
  memset(e, 0, sizeof(Entity));
  e->x = x;
  e->y = y;
  e->w = w;
  e->h = h;
}

void entity_update(Entity* e) {
  e->x += e->x_speed;
  e->y += e->y_speed;
  if (e->x < 0) {
    e->x_speed = -e->x_speed;
    e->x = 0;
  }
  if (e->x > 250) {
    e->x_speed = -e->x_speed;
    e->x = 250;
  }
}

void entity_render(Entity* e) {
  u32 texture = 1;  // NOTE(lucas): Temp!
  render_rect(e->x, e->y, 0, e->w, e->h, 0.9f, 0.1f, 0.12f, 0, 1.0f / (e->w));
  render_texture_region(texture, e->x, e->y, e->w, e->h, 0, 0, 0, 8, 8, 48, 8);
}
