// entity.c

#include "common.h"
#include "game_common.h"
#include "renderer.h"
#include "game.h"
#include "resource.h"
#include "audio.h"
#include "entity.h"

#define TEXT_BUFF_SIZE (96)
#define MOVE_INTERVAL (15)

static i16 id_count = 0;
static char temp_text[TEXT_BUFF_SIZE];

static void entity_move(Entity* e);

void entity_move(Entity* e) {
  Tile* tile = tilemap_get_tile(&game_state.tile_map, e->x_tile + e->x_dir, e->y_tile + e->y_dir);
  u8 collision = 0;
  if (!tile) {
    e->x_dir = -e->x_dir;
    e->y_dir = -e->y_dir;
    collision = 1;
  }
  else if (tile->tile_type == TILE_BRICK) {
    e->x_dir = -e->x_dir;
    e->y_dir = -e->y_dir;
    collision = 1;
  }
  else {
    e->x_tile += e->x_dir;
    e->y_tile += e->y_dir;
  }
  if (collision) {
    e->health--;
    if (e->health <= 0) {
      e->health = 0;
      e->state = STATE_DEAD;
      audio_play_once(SOUND_HIT, 0.5f);
    }
    else {
      audio_play_once(SOUND_GOOD_MORNING, 0.5f);
    }
    renderer_set_tint(15, 15, 15, 1);
  }
  else {
    renderer_set_tint(1, 1, 1, 1);
  }
}

void entity_init(Entity* e, float x, float y, float w, float h) {
  memset(e, 0, sizeof(Entity));
  e->x = x;
  e->y = y;
  e->w = w;
  e->h = h;
  e->x_dir = 0;
  e->y_dir = 0;
  e->state = STATE_ALIVE;
  e->e_flags = 0;
  e->type = 0;
  e->tile_type = 0;
  e->sprite_id = rand() % 6;
  e->id = id_count++;
}

void entity_init_tilepos(Entity* e, i32 x_tile, i32 y_tile, float w, float h) {
  entity_init(e, x_tile * TILE_SIZE, y_tile * TILE_SIZE, w, h);
  e->x_tile = x_tile;
  e->y_tile = y_tile;
}

#define INTERP_MOVEMENT 1

void entity_update_and_render(Entity* e) {
  if (e->state == STATE_DEAD) {
    game_entity_remove(e);
    e->state = STATE_NONE;
    return;
  }
  if (!(game_state.tick % MOVE_INTERVAL)) {
    entity_move(e);
  }
#if INTERP_MOVEMENT
  e->x = lerp(e->x, TILE_SIZE * e->x_tile, 0.25f);
  e->y = lerp(e->y, TILE_SIZE * e->y_tile, 0.25f);
#else
  e->x = TILE_SIZE * e->x_tile;
  e->y = TILE_SIZE * e->y_tile;
#endif
  if (e->e_flags & ENTITY_FLAG_DRAW_HEALTH) {
    i32 w = e->w * 0.8f;
    i32 h = 8;
    i32 x_pos = (e->x + (e->w / 2.0f)) - (w / 2.0f),
      y_pos = e->y - 10;
    render_filled_rectangle(x_pos - camera.x, y_pos - camera.y, 0.15f, (w * ((float)e->health / e->max_health)), h, 0.2f, 0.85f, 0.2f, 1.0f, 0.2f, 0.2f, 0.5f, 1.0f, 0, 1.0f / w);
    render_filled_rectangle(x_pos - camera.x, y_pos - camera.y, 0.15f, w, h, 0.85f, 0.2f, 0.2f, 1.0f, 0.5f, 0.2f, 0.2f, 1.0f, 0, 1.0f / w);
  }
  render_texture_region(textures[TEXTURE_SPRITES], e->x - camera.x, e->y - camera.y, 0, e->w, e->h, 0, e->sprite_id * 8, 0, 8, 8);
}

void entity_render_highlight(Entity* e) {
  render_rect(e->x - camera.x, e->y - camera.y, 0.1f, e->w, e->h, 0.9f, 0.1f, 0.12f, 1.0f, 0, 1.0f / (e->w));
  snprintf(temp_text, TEXT_BUFF_SIZE, "id=%i\nx=%i\ny=%i\nhp: %i/%i", e->id, (i32)e->x, (i32)e->y, e->health, e->max_health);
  render_text(textures[TEXTURE_FONT],
    e->x - camera.x + e->w + 2,
    e->y - camera.y + e->h + 2, 0.2f, 130, 90, 14, 0.7f, 0.5f, 6, temp_text, TEXT_BUFF_SIZE);
}

void entity_render_highlight_color(Entity* e, float r, float g, float b) {
  render_rect(e->x - camera.x, e->y - camera.y, 0.1f, e->w, e->h, r, g, b, 1.0f, 0, 1);
}
