// level.h

#ifndef _LEVEL_H
#define _LEVEL_H

#define MAX_ENTITY (256)

typedef struct Level {
  u32 index;
  u32 entity_count;
  struct Entity entities[MAX_ENTITY];
  struct Tile_map tile_map;
} Level;

void level_init(Level* level);

i32 level_store(Level* level, u32 index);

i32 level_load(Level* level, u32 index);

#endif
