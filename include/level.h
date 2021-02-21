// level.h

#ifndef _LEVEL_H
#define _LEVEL_H

#define MAX_ENTITY (256)

typedef struct Level {
  u32 index;
  u32 entity_count;
  struct Tile_map tile_map;
  struct Entity_def entities[MAX_ENTITY];
} Level;

typedef struct World {
  Level level;
  struct Entity entities[MAX_ENTITY];
  u32 entity_count;
} World;

typedef struct Save_state {
  i32 score;
  i32 level;
} Save_state;

void save_state_init(Save_state* state);

i32 save_state_store(Save_state* state);

i32 save_state_load(Save_state* state);

void level_init(Level* level);

i32 level_store(World* world, u32 index);

i32 level_load(World* world, u32 index);

#endif
