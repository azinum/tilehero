// entity.c

#include "common.h"
#include "renderer.h"
#include "game.h"
#include "resource.h"
#include "audio.h"
#include "entity.h"

#define TEXT_BUFF_SIZE (96)

static i16 id_count = 0;
static char temp_text[TEXT_BUFF_SIZE];


void entity_init(Entity* e, float x, float y, float w, float h) {
  memset(e, 0, sizeof(Entity));
  e->x = x;
  e->y = y;
  e->w = w;
  e->h = h;
  e->type = 0;
  e->tile_type = 0;
  e->sprite_id = rand() % 6;
  e->id = id_count++;
}

void entity_update(Entity* e) {
  u8 collision = 0;
  e->x += e->x_speed;
  e->y += e->y_speed;
  if (e->x < 0) {
    e->x_speed = -e->x_speed;
    e->x = 0;
    collision = 1;
  }
  if (e->x > 250) {
    e->x_speed = -e->x_speed;
    e->x = 250;
    collision = 1;
  }
  if (e->y < 0) {
    e->y_speed = -e->y_speed;
    e->y = 0;
    collision = 1;
  }
  if (e->y > 250) {
    e->y_speed = -e->y_speed;
    e->y = 250;
    collision = 1;
  }

  if (collision) {
    audio_play_once(e->sprite_id % (MAX_SOUND - 1), 0.15f);
    renderer_set_tint(15, 15, 15, 1);
  }
  else {
    renderer_set_tint(1, 1, 1, 1);
  }
}

void entity_render(Entity* e) {
  render_texture_region(textures[TEXTURE_SPRITES], e->x - camera.x, e->y - camera.y, 0, e->w, e->h, 0, e->sprite_id * 8, 0, 8, 8);
}

void entity_render_highlight(Entity* e) {
  render_rect(e->x - camera.x, e->y - camera.y, 0.1f, e->w, e->h, 0.9f, 0.1f, 0.12f, 1.0f, 0, 1.0f / (e->w));
  snprintf(temp_text, TEXT_BUFF_SIZE, "id=%i\nx=%i\ny=%i", e->id, (i32)e->x, (i32)e->y);
  render_text(textures[TEXTURE_FONT],
    e->x - camera.x + 32,
    e->y - camera.y + 32, 0.1f, 100, 76, e->w / 2, 0.7f, 0.5f, 6, temp_text, TEXT_BUFF_SIZE);
}

void entity_render_highlight_color(Entity* e, float r, float g, float b) {
  render_rect(e->x - camera.x, e->y - camera.y, 0.1f, e->w, e->h, r, g, b, 1.0f, 0, 1);
}
