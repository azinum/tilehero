// entity.c

#include "common.h"
#include "game_common.h"
#include "renderer.h"
#include "game.h"
#include "resource.h"
#include "audio.h"
#include "player.h"
#include "editor.h"
#include "entity.h"

#define TEXT_BUFF_SIZE 96
#define INTERP_MOVEMENT 1
#define INTERP_SPEED 20.0f

struct Tile_move tile_moves[MAX_MOVES];
u32 move_count = 0;
float move_time = 0;
static i16 id_count = 0;
static char temp_text[TEXT_BUFF_SIZE];

void entity_init(Entity* e, float x, float y, float w, float h) {
  memset(e, 0, sizeof(Entity));

  e->id = id_count++;
  e->x_tile = 0;
  e->y_tile = 0;
  e->target_id = -1;
  e->placable_id = -1;

  e->x = x;
  e->y = y;
  e->w = w;
  e->h = h;
  e->x_dir = 0;
  e->y_dir = 0;
  e->state = STATE_ACTIVE;
  e->e_flags = 0;
  e->type = 0;
  e->sprite_id = 0;
  e->health = e->max_health = 0;
  e->attack = 0;
  e->xp = 0;
}

void entity_hurt_from_foe(Entity* e, Entity* foe) {
  e->health -= foe->attack;
  if (e->health > e->max_health)
    e->health = e->max_health;
  if (e->health <= 0) {
    e->health = 0;
    e->state = STATE_DEAD;
    foe->xp += e->max_health * 4;
    audio_play_once(SOUND_HIT, SFX_VOLUME);
    if (e->type == ENTITY_TYPE_PLAYER) {
      game_send_message(M_DIED);
      game_restart();
      return;
    }
  }
  else {
    audio_play_once(SOUND_HURT, SFX_VOLUME);
  }
}

// Copypasta from entity_hurt_from_foe. Make better, unify.
void entity_hurt(Entity* e, i16 amount) {
  e->health -= amount;
  if (e->health > e->max_health)
    e->health = e->max_health;
  if (e->health <= 0) {
    e->health = 0;
    e->state = STATE_DEAD;
    audio_play_once(SOUND_HIT, SFX_VOLUME);
    if (e->type == ENTITY_TYPE_PLAYER) {
      game_send_message(M_DIED);
      game_restart();
      return;
    }
  }
  else {
    audio_play_once(SOUND_HURT, SFX_VOLUME);
  }
}

void entity_tiled_move(struct Entity* e) {
  if (move_count >= MAX_MOVES) {
    fprintf(stderr, "Move count limit reached!\n");
    game_send_message("[ERROR]: MOVE COUNT LIMIT REACHED!");
    return;
  }
  if (!e->x_dir && !e->y_dir)
    return;
  struct Tile_move move = {
    .x_tile = e->x_tile + e->x_dir,
    .y_tile = e->y_tile + e->y_dir,
    .entity = e,
  };
  tile_moves[move_count++] = move;
}

i32 entity_to_def(Entity* e, struct Entity_def* def) {
  if (e->placable_id < 0 || e->placable_id >= MAX_PLACABLE_ENTITY) {
    // Ok, this is no storage entity
    return ERR;
  }
  def->id = e->id;
  def->x_tile = e->x_tile;
  def->y_tile = e->y_tile;
  def->target_id = e->target_id;
  def->placable_id = e->placable_id;
  return NO_ERR;
}

void entity_do_tiled_move(Entity* entities, i32 entity_count, Level* level) {
  for (u32 i = 0; i < move_count; i++) {
    struct Tile_move* move = &tile_moves[i];
    if (!move->entity)    continue;

    Entity* e = move->entity;
    Entity* target = NULL;
    u8 collision = 0;
    for (i32 entity_index = 0; entity_index < entity_count; entity_index++) {
      Entity* current = &entities[entity_index];
      if (current == e)   continue;

      if (move->x_tile == current->x_tile && move->y_tile == current->y_tile) {
        target = current;
        collision = 1;
        break;
      }
    }

    i32 can_move_after = 0;
    Tile* tile = tilemap_get_tile(&level->tile_map, move->x_tile, move->y_tile);

    if (!tile) {  // Outside the map
      collision = 1;
    }
    else if (!tile->walkable && !(e->e_flags & ENTITY_FLAG_FLY && tile->type == TILE_VOID)) {
      collision = 1;
    }

    if (collision) {
      if (e->type == ENTITY_TYPE_NPC || e->type == ENTITY_TYPE_FLAG) {
        e->x_dir = -e->x_dir;
        e->y_dir = -e->y_dir;
      }
      if (target) {
        if (target->type == ENTITY_TYPE_SILVER_KEY) {
          if (e->type == ENTITY_TYPE_NPC) {
            target->state = STATE_DEAD;
            game_send_message(M_SILVER_KEY_DESTROYED);
          }
        }
        if (e->type == ENTITY_TYPE_SILVER_KEY && target->type == ENTITY_TYPE_SILVER_DOOR) {
          e->state = STATE_DEAD;
          target->state = STATE_DEAD;
        }
        if (target->e_flags & ENTITY_FLAG_PUSHABLE) {
          target->x_dir = e->x_dir;
          target->y_dir = e->y_dir;
          entity_tiled_move(target);
          target->x_dir = 0;
          target->y_dir = 0;
#if 1
          can_move_after = 1;
#else
          entity_tiled_move(e);
          e->x_dir = 0;
          e->y_dir = 0;
#endif
        }
        if (target->type == ENTITY_TYPE_FLAG && e->type == ENTITY_TYPE_PLAYER) {
          game_send_message("Level %i complete!", level->index);
          game_load_level_on_complete(level->index + 1);
          return;
        }
#if 0
        if (e->type == ENTITY_TYPE_CONSUMABLE) {
          target->health -= e->attack;
          if (target->health > target->max_health) {
            target->health = target->max_health;
          }
          if (target->health <= 0) {
            target->health = 0;
            target->state = STATE_DEAD;
          }
          e->state = STATE_DEAD;
        }
        if (target->type == ENTITY_TYPE_CONSUMABLE && e->id != target->id) {
          e->health -= target->attack;
          if (e->health > e->max_health) {
            e->health = e->max_health;
          }
          if (e->health <= 0) {
            e->health = 0;
            e->state = STATE_DEAD;
          }
          target->state = STATE_DEAD;
        }
#endif
        if (!(e->e_flags & ENTITY_FLAG_FRIENDLY) && !(target->e_flags & ENTITY_FLAG_FRIENDLY) && !(e->type == ENTITY_TYPE_PLAYER && target->type == ENTITY_TYPE_PLAYER) && !(e->type == target->type)) {
          entity_hurt_from_foe(target, e);
        }
      }
      else { // We hit a non-walkable tile!
        switch (e->type) {
          case ENTITY_TYPE_CONSUMABLE: {
            e->health--;
            if (e->health <= 0) {
              e->state = STATE_DEAD;
            }
            break;
          }
          case ENTITY_TYPE_PUSHER: {
            i8 x_dir = move->x_tile - e->x_tile;
            i8 y_dir = move->y_tile - e->y_tile;
            Tile* current = tilemap_get_tile(&level->tile_map, move->x_tile, move->y_tile);
            Tile* next = tilemap_get_tile(&level->tile_map, move->x_tile + x_dir, move->y_tile + y_dir);
            Tile* prev = tilemap_get_tile(&level->tile_map, move->x_tile - x_dir, move->y_tile - y_dir);
            if (current && next && prev) {
              Tile tmp = *current;
              *current = *prev;
              *next = tmp;
              audio_play_once(SOUND_CRUNCH, SFX_VOLUME * 0.8f);
              e->state = STATE_DEAD;
            }
            break;
          }
        }
      }
    }
    else if (tile) {  // No collision, let's move to this tile!
      e->x_tile = move->x_tile;
      e->y_tile = move->y_tile;
      switch (tile->type) {
        case TILE_DUNGEON: {
          i8 x_dir = e->x_dir;
          i8 y_dir = e->y_dir;
          if (abs(e->x_dir) >= 1) {
            e->y_dir = x_dir;
            e->x_dir = 0;
          }
          else if (abs(e->y_dir) >= 1) {
            e->x_dir = y_dir;
            e->y_dir = 0;
          }
          if (e->type == ENTITY_TYPE_PLAYER) {
            // player.stunned++;
            // game_send_message("You have been stunned for %i moves(s)", player.stunned);
          }
          break;
        }
        case TILE_SWAPPER: {
          Tile* l = tilemap_get_tile(&level->tile_map, move->x_tile - 1, move->y_tile); // Left
          Tile* r = tilemap_get_tile(&level->tile_map, move->x_tile + 1, move->y_tile); // Right
          Tile* t = tilemap_get_tile(&level->tile_map, move->x_tile, move->y_tile - 1); // Top
          Tile* b = tilemap_get_tile(&level->tile_map, move->x_tile, move->y_tile + 1); // Bottom
          if (l && r) {
            Tile l_tmp = *l;
            *l = *r;
            *r = l_tmp;
          }
          if (t && b) {
            Tile t_tmp = *t;
            *t = *b;
            *b = t_tmp;
          }
          audio_play_once(SOUND_RANDOM_1, SFX_VOLUME);
          break;
        }
        case TILE_PORTAL: {
          if (e->type != ENTITY_TYPE_PLAYER) {
            game_copy_add_entity(*e);
            audio_play_once(SOUND_BLIP, SFX_VOLUME * 0.4f);
            i8 x_dir = e->x_dir;
            i8 y_dir = e->y_dir;
            e->x_dir = y_dir;
            e->y_dir = -x_dir;
            // *tile = placable_tiles[rand() % MAX_TILE];
          }
          break;
        }
      }
    }
    else {
      e->x_tile = move->x_tile;
      e->y_tile = move->y_tile;
    }
    // Ok, this is pretty funky! Has bugs as well. TODO(lucas): Fix, and rework the movement system
    if (can_move_after) {
      e->x_tile = move->x_tile;
      e->y_tile = move->y_tile;
    }
  }
  move_count = 0;
}

void entity_init_tilepos(Entity* e, i32 x_tile, i32 y_tile) {
  e->x_tile = x_tile;
  e->y_tile = y_tile;
  e->x = x_tile * TILE_SIZE;
  e->y = y_tile * TILE_SIZE;
}

void entity_update(Entity* e) {
  if (e->state == STATE_DEAD) {
    game_entity_remove(e);
    return;
  }
  if (e->e_flags & ENTITY_FLAG_MOVABLE) {
    if (game_state.should_move) {
      entity_tiled_move(e);
    }
#if INTERP_MOVEMENT
    e->x = lerp2(e->x, (TILE_SIZE * e->x_tile), INTERP_SPEED * game_state.delta_time * game_state.time_scale, 0.1f);
    e->y = lerp2(e->y, (TILE_SIZE * e->y_tile), INTERP_SPEED * game_state.delta_time * game_state.time_scale, 0.1f);
#else
    e->x = TILE_SIZE * e->x_tile;
    e->y = TILE_SIZE * e->y_tile;
#endif
  }
}

void entity_render(Entity* e) {
  if (e->e_flags & ENTITY_FLAG_DRAW_HEALTH) {
    i32 w = e->w * 0.8f;
    i32 h = 8;
    i32 x_pos = (e->x + ((i32)e->w >> 1)) - (w >> 1),
    y_pos = e->y - 12;
    render_filled_rect(x_pos - (i32)camera.x, y_pos - (i32)camera.y, 0.15f, (i32)(w * ((float)e->health / e->max_health)), h, 0.2f, 0.85f, 0.2f, 1.0f, 0);
    render_filled_rectangle(x_pos - (i32)camera.x, y_pos - (i32)camera.y, 0.15f, w, h, 0.65f, 0.2f, 0.2f, 1.0f, 0.5f, 0.2f, 0.2f, 1.0f, 0, 1.0f / w);
  }
  render_sprite(SHEET_ENTITIES, e->sprite_id, e->x - camera.x, e->y - camera.y, 0, e->w, e->h);
}

// TODO(lucas): Seperate this out to the editor instead
void entity_render_highlight(Entity* e) {
  render_rect(e->x - camera.x, e->y - camera.y, 0.1f, e->w, e->h, 0.9f, 0.1f, 0.12f, 1.0f, 0, 2.0f / (e->w));
  snprintf(temp_text, TEXT_BUFF_SIZE, "id: %i\nx: %i\ny: %i\nhp: %i/%i\nattack: %i\ndir: %i, %i\np_id: %i\n", e->id, e->x_tile, e->y_tile, e->health, e->max_health, e->attack, e->x_dir, e->y_dir, e->placable_id);
  render_text(textures[TEXTURE_FONT],
    e->x - camera.x + e->w + 2,
    e->y - camera.y + e->h + 2, 0.2f, 130, 120, V3(1, 1, 1), 14, 0.7f, 0.5f, 6, temp_text, TEXT_BUFF_SIZE,
    1,
    0, 0, 0, 1, // Background color
    0.8f, 0.1f, 0.31f, 1.0f,  // Border color
    2.0f  // Border thickness
  );
}

void entity_render_highlight_color(Entity* e, float r, float g, float b, float a) {
  render_rect(e->x - camera.x, e->y - camera.y, 0.1f, e->w, e->h, r, g, b, a, 0, 2.0f / (e->w));
}
