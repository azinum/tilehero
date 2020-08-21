// world.h

#ifndef _WORLD_H
#define _WORLD_H

#define MAX_ENTITY (256)

typedef struct World_position {
  i32 x, y, z;
} World_position;

#define WORLD_VEC3(X, Y, Z) ((World_position) {X, Y, Z})

typedef struct World_chunk {
  World_position position;
  u32 chunk_index;
  u32 entity_count;
  struct Tile_map tile_map;
  struct Entity entities[MAX_ENTITY];
} World_chunk;

#define NUM_WORLD_CHUNKS 4096

void world_chunk_init(struct World_chunk* chunk, World_position position);

i32 world_chunk_store_hashed(struct World_chunk* chunk, const char* world_storage_file);

i32 world_chunk_load_hashed(struct World_chunk* chunk, World_position position, const char* world_storage_file);

#endif
