// world.h

#ifndef _WORLD_H
#define _WORLD_H

#define MAX_ENTITY (256)

typedef struct World_chunk {
  struct Tile_map tile_map;
  u32 entity_count;
  struct Entity entities[MAX_ENTITY];
  u32 chunk_index;
} World_chunk;

#define NUM_WORLD_CHUNKS 4096

struct World {
  struct World_chunk chunks[NUM_WORLD_CHUNKS];
};

i32 world_chunk_store(struct World_chunk* chunk, const char* world_storage_file);

i32 world_chunk_load(struct World_chunk* chunk, u32 chunk_index, const char* world_storage_file);

#endif
