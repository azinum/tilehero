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

// TODO(lucas): Move this elsewhere?
struct Tile_move tile_moves[MAX_MOVES];
i32 move_count;

static i16 id_count = 0;
static char temp_text[TEXT_BUFF_SIZE];

i32 entity_store(Entity* entities, u32 count, const char* path) {
  FILE* fp = fopen(path, "wb");
  if (!fp) {
    fprintf(stderr, "Failed to open file '%s'\n", path);
    return -1;
  }
  u32 bytes_written = 0;
  bytes_written += fwrite(&count, 1, sizeof(count), fp);
  bytes_written += fwrite(entities, 1, count * sizeof(Entity), fp);
  if ((sizeof(count) + count * sizeof(Entity)) != bytes_written) {
    fprintf(stderr, "Failed to write file '%s'\n", path);
    fclose(fp);
    return -1;
  }
  printf("Entities stored to file '%s'\n", path);
  fclose(fp);
  return 0;
}

i32 entity_load(Entity* entities, u32* count, const char* path) {
  FILE* fp = fopen(path, "rb");
  if (!fp) {
    fprintf(stderr, "Failed to open file '%s'\n", path);
    return -1;
  }
  fseek(fp, 0, SEEK_END);
  u32 size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  if (size < sizeof(u32)) {
    fprintf(stderr, "Invalid entity file '%s'\n", path);
    fclose(fp);
    return -1;
  }

  u32 bytes_read = 0;
  bytes_read += fread(count, 1, sizeof(u32), fp);
  if ((*count) < (u32)(size / sizeof(Entity))) {
    fprintf(stderr, "Invalid entity file '%s'\n", path);
    fclose(fp);
    return -1;
  }
  bytes_read += fread(entities, 1, *count * sizeof(Entity), fp);
  if (bytes_read != (sizeof(u32) + (*count * sizeof(Entity)))) {
    fprintf(stderr, "Failed to read entity file '%s'\n", path);
  }
  fclose(fp);
  return 0;
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
        Tile* tile = tilemap_get_tile(&game_state.world_chunk.tile_map, move->x_tile, move->y_tile);
        if (!tile) {  // Outside the map
            collision = 1;
        }
        else if (tile->tile_type != TILE_DEFAULT && tile->tile_type != TILE_SWAPPER && tile->tile_type != TILE_DUNGEON) { // Make sure we don't hit a walkable tile
            collision = 1;
        }
        
        if (collision) {
            e->x_dir = -e->x_dir;
            e->y_dir = -e->y_dir;
            if (target) {
                if (!(e->e_flags & ENTITY_FLAG_FRIENDLY) && !(target->e_flags & ENTITY_FLAG_FRIENDLY)) {
#if 1
                    target->health -= e->attack;
                    if (target->health <= 0) {
                        target->health = 0;
                        target->state = STATE_DEAD;
                        e->xp += target->max_health * 4;
                        e->max_health += 0.1756f * target->max_health;
                        e->health += target->max_health * 0.8f;
                        if (e->health >= e->max_health)
                            e->health = e->max_health;
                        e->attack++;
                        audio_play_once(SOUND_HIT, 0.5f);
                    }
                    else {
                        audio_play_once(SOUND_GOOD_MORNING, 0.5f);
                    }
#endif
                }
            }
        }
        else {  // No collision, let's move to this tile!
            e->x_tile = move->x_tile;
            e->y_tile = move->y_tile;
            switch (tile->tile_type) {
                case TILE_DUNGEON: {
                    if (abs(e->x_dir) == 1) {
                        e->y_dir = e->x_dir;
                        e->x_dir = 0;
                    }
                    else if (abs(e->y_dir) == 1) {
                        e->x_dir = e->y_dir;
                        e->y_dir = 0;
                    }
                    break;
                }
                // TODO(lucas): Find a way to do this more elegantly!
                case TILE_SWAPPER: {
                    Tile* l = tilemap_get_tile(&game_state.world_chunk.tile_map, move->x_tile - 1, move->y_tile); // Left
                    Tile* r = tilemap_get_tile(&game_state.world_chunk.tile_map, move->x_tile + 1, move->y_tile); // Right
                    Tile* t = tilemap_get_tile(&game_state.world_chunk.tile_map, move->x_tile, move->y_tile - 1); // Top
                    Tile* b = tilemap_get_tile(&game_state.world_chunk.tile_map, move->x_tile, move->y_tile + 1); // Bottom
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
                    audio_play_once(SOUND_RANDOM_1, 0.5f);
                    break;
                }
            }
        }
    }
    move_count = 0;
}

void entity_init_tilepos(Entity* e, i32 x_tile, i32 y_tile, float w, float h) {
    entity_init(e, x_tile * TILE_SIZE, y_tile * TILE_SIZE, w, h);
    e->x_tile = x_tile;
    e->y_tile = y_tile;
}

void entity_update(Entity* e) {
    if (e->state == STATE_DEAD) {
        game_entity_remove(e);
        return;
    }
    if (game_state.time >= game_state.move_timer) {
        entity_tiled_move(e);
    }
#if INTERP_MOVEMENT
    e->x = lerp(e->x, TILE_SIZE * e->x_tile, INTERP_SPEED * game_state.delta_time * game_state.time_scale);
    e->y = lerp(e->y, TILE_SIZE * e->y_tile, INTERP_SPEED * game_state.delta_time * game_state.time_scale);
#else
    e->x = TILE_SIZE * e->x_tile;
    e->y = TILE_SIZE * e->y_tile;
#endif
}

void entity_render(Entity* e) {
    if (e->e_flags & ENTITY_FLAG_DRAW_HEALTH) {
        i32 w = e->w * 0.8f;
        i32 h = 8;
        i32 x_pos = (e->x + (e->w / 2.0f)) - (w / 2.0f),
        y_pos = e->y - 10;
        render_filled_rectangle((i32)(x_pos - camera.x), (i32)(y_pos - camera.y), 0.15f, (i32)(w * ((float)e->health / e->max_health)), h, 0.2f, 0.85f, 0.2f, 1.0f, 0.2f, 0.2f, 0.5f, 1.0f, 0, 1.0f / w);
        render_filled_rectangle((i32)(x_pos - camera.x), (i32)(y_pos - camera.y), 0.15f, w, h, 0.85f, 0.2f, 0.2f, 1.0f, 0.5f, 0.2f, 0.2f, 1.0f, 0, 1.0f / w);
    }
    struct Sprite_info sprite = sprite_info[e->sprite_id];
    render_texture_region(textures[TEXTURE_SPRITES], e->x - camera.x, e->y - camera.y, 0, e->w, e->h, 0, sprite.x_offset, sprite.y_offset, sprite.x_range, sprite.y_range);
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
