// level.h

#ifndef _LEVEL_H
#define _LEVEL_H

#define MAX_ENTITY (256)

typedef struct Level {
  u32 index;
  u32 entity_count;
  struct Tile_map tile_map;
  struct Entity_def entities[MAX_ENTITY]; // Need to seperate the world data file into multiple files if we want to, with flexibility, have dynamic size of entity storage
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

void level_init(Level* level);

i32 level_store(World* world, u32 index);

i32 level_load(World* world, u32 index);

#endif
