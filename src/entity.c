// entity.c

#include "common.h"
#include "game_common.h"
#include "renderer.h"
#include "game.h"
#include "resource.h"
#include "audio.h"
#include "entity.h"

#define TEXT_BUFF_SIZE (96)
#define MAX_MOVES (256)
#define INTERP_MOVEMENT 1
#define INTERP_SPEED (20.0f)

struct Tile_move {
  i32 x_tile;
  i32 y_tile;
  struct Entity* entity;
};

static struct Tile_move tile_moves[MAX_MOVES];
static i32 move_count;

static i16 id_count = 0;
static char temp_text[TEXT_BUFF_SIZE];

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
  e->sprite_id = 0;
  e->id = id_count++;
  e->health = e->max_health = 0;
  e->attack = 0;
  e->xp = 0;
}

void entity_tiled_move(struct Entity* e) {
  if (move_count >= MAX_MOVES) {
    assert(0);
    return;
  }
  struct Tile_move move = {
    .x_tile = e->x_tile + e->x_dir,
    .y_tile = e->y_tile + e->y_dir,
    .entity = e
  };
  tile_moves[move_count++] = move;
}

void entity_do_tiled_move(Entity* entities, i32 entity_count) {
  for (i32 i = 0; i < move_count; i++) {
    struct Tile_move* move = &tile_moves[i];
    Entity* e = move->entity;
    Entity* target = NULL;
    u8 collision = 0;
    for (i32 entity_index = 0; entity_index < entity_count; entity_index++) {
      Entity* current = &entities[entity_index];
      if (current == e)
        continue;
      if (move->x_tile == current->x_tile && move->y_tile == current->y_tile) {
        target = current;
        collision = 1;
        break;  
      }
    }
    Tile* tile = tilemap_get_tile(&game_state.tile_map, move->x_tile, move->y_tile);
    if (!tile) {  // Outside the map
      collision = 1;
    }
    else if (tile->tile_type != TILE_DEFAULT && tile->tile_type != TILE_DUNGEON) { // We hit a tile
      collision = 1;
    }

    if (collision) {
      e->x_dir = -e->x_dir;
      e->y_dir = -e->y_dir;
      if (target) {
        if (!(e->e_flags & ENTITY_FLAG_FRIENDLY) && !(target->e_flags & ENTITY_FLAG_FRIENDLY)) {
          target->health -= e->attack;
          if (target->health <= 0) {
            target->health = 0;
            target->state = STATE_DEAD;
            e->xp += target->max_health * 4;
            e->max_health += 0.1756f * target->max_health;
            e->health += target->max_health / 4;
            e->attack++;
            audio_play_once(SOUND_HIT, 0.5f);
          }
          else {
            audio_play_once(SOUND_GOOD_MORNING, 0.5f);
          }
          renderer_set_tint(15, 15, 15, 1);
        }
      }
    }
    else {  // No collision, let's move to this tile!
      e->x_tile = move->x_tile;
      e->y_tile = move->y_tile;
      renderer_set_tint(1, 1, 1, 1);
    }
  }
  move_count = 0;
}

void entity_init_tilepos(Entity* e, i32 x_tile, i32 y_tile, float w, float h) {
  entity_init(e, x_tile * TILE_SIZE, y_tile * TILE_SIZE, w, h);
  e->x_tile = x_tile;
  e->y_tile = y_tile;
}

void entity_update_and_render(Entity* e) {
  if (game_state.mode == MODE_GAME) { // @TEMP
    if (e->state == STATE_DEAD) {
      game_entity_remove(e);
      return;
    }
    if (game_state.time >= game_state.move_timer) {
      entity_tiled_move(e);
    }
#if INTERP_MOVEMENT
    e->x = lerp(e->x, TILE_SIZE * e->x_tile, INTERP_SPEED * game_state.delta_time);
    e->y = lerp(e->y, TILE_SIZE * e->y_tile, INTERP_SPEED * game_state.delta_time);
#else
    e->x = TILE_SIZE * e->x_tile;
    e->y = TILE_SIZE * e->y_tile;
#endif
  }
  if (e->e_flags & ENTITY_FLAG_DRAW_HEALTH) {
    i32 w = e->w * 0.8f;
    i32 h = 8;
    i32 x_pos = (e->x + (e->w / 2.0f)) - (w / 2.0f),
      y_pos = e->y - 10;
    render_filled_rectangle(x_pos - camera.x, y_pos - camera.y, 0.15f, (w * ((float)e->health / e->max_health)), h, 0.2f, 0.85f, 0.2f, 1.0f, 0.2f, 0.2f, 0.5f, 1.0f, 0, 1.0f / w);
    render_filled_rectangle(x_pos - camera.x, y_pos - camera.y, 0.15f, w, h, 0.85f, 0.2f, 0.2f, 1.0f, 0.5f, 0.2f, 0.2f, 1.0f, 0, 1.0f / w);
  }
  // render_texture_region(textures[TEXTURE_SPRITES], e->x - camera.x, e->y - camera.y, 0, e->w, e->h, 0, e->sprite_id * 8, 0, 8, 8);
  render_texture_region(textures[TEXTURE_SPRITES], e->x - camera.x, e->y - camera.y, 0, e->w, e->h, 0, e->sprite_id * 8, 0, 8, 8);
}

void entity_render_highlight(Entity* e) {
  render_rect(e->x - camera.x, e->y - camera.y, 0.1f, e->w, e->h, 0.9f, 0.1f, 0.12f, 1.0f, 0, 1.0f / (e->w));
  snprintf(temp_text, TEXT_BUFF_SIZE, "id=%i\nx=%i\ny=%i\nhp: %i/%i\nattack: %i\nxp: %i", e->id, (i32)e->x, (i32)e->y, e->health, e->max_health, e->attack, e->xp);
  render_text(textures[TEXTURE_FONT],
    e->x - camera.x + e->w + 2,
    e->y - camera.y + e->h + 2, 0.2f, 130, 105, 14, 0.7f, 0.5f, 6, temp_text, TEXT_BUFF_SIZE);
}

void entity_render_highlight_color(Entity* e, float r, float g, float b) {
  render_rect(e->x - camera.x, e->y - camera.y, 0.1f, e->w, e->h, r, g, b, 1.0f, 0, 1);
}
