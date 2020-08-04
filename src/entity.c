// entity.c

#include "common.h"
#include "renderer.h"
#include "game.h"
#include "resource.h"
#include "audio.h"
#include "entity.h"

void entity_init(Entity* e, float x, float y, float w, float h) {
  memset(e, 0, sizeof(Entity));
  e->x = x;
  e->y = y;
  e->w = w;
  e->h = h;
  e->sprite_id = rand() % 6;
}

void entity_update(Entity* e) {
  u8 collided = 0;
  e->x += e->x_speed;
  e->y += e->y_speed;
  if (e->x < 0) {
    e->x_speed = -e->x_speed;
    e->x = 0;
    collided = 1;
  }
  if (e->x > 250) {
    e->x_speed = -e->x_speed;
    e->x = 250;
    collided = 1;
  }
  if (e->y < 0) {
    e->y_speed = -e->y_speed;
    e->y = 0;
    collided = 1;
  }
  if (e->y > 250) {
    e->y_speed = -e->y_speed;
    e->y = 250;
    collided = 1;
  }

  if (collided) {
    audio_play_once(rand() % MAX_SOUND, 0.2f);
  }
}

void entity_render(Entity* e) {
  render_texture_region(textures[TEXTURE_SPRITES], e->x - camera.x, e->y - camera.y, camera.z, e->w, e->h, 0, e->sprite_id * 8, 0, 8, 8);
}

void entity_render_highlight(Entity* e) {
  render_rect(e->x - camera.x, e->y - camera.y, 0.1f, e->w, e->h, 0.9f, 0.1f, 0.12f, 0, 1.0f / (e->w));
}
