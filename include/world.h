// world.h

#ifndef _WORLD_H
#define _WORLD_H

#define MAX_ENTITY (256)

typedef struct World_position {
  i32 x, y, z;
} World_position;

#define WORLD_VEC3(X, Y, Z) ((World_position) {X, Y, Z})
#define WORLD_VEC3_EQUAL(A, B) (A.x == B.x && A.y == B.y && A.z == B.z)

typedef struct World_chunk {
  World_position position;
  u32 chunk_index;
  u32 entity_count;
  struct Tile_map tile_map;
  struct Entity entities[MAX_ENTITY];
} World_chunk;

#define NUM_WORLD_CHUNKS 4096
#define CHUNK_AREA_X 3
#define CHUNK_AREA_Y 3
#define NUM_LOADED_WORLD_CHUNKS (CHUNK_AREA_X * CHUNK_AREA_Y)

typedef struct World {
  World_position current_origin;
  World_chunk chunks[NUM_LOADED_WORLD_CHUNKS];

  // NOTE(lucas): These are the entities in which we are currently processing
  Entity* entities;
  u32 entity_count;

  Entity swap_entities[MAX_ENTITY];
  u32 swap_entity_count;
} World;

void world_chunk_init(World_chunk* chunk, World_position position);

void world_load_chunks_from_origin(World* world, World_position origin);

i32 world_add_entity_to_chunk(World_chunk* chunk, Entity* e);

i32 world_transfer_entities_to_chunks(World* world);

i32 world_chunk_store(World_chunk* chunk, i32 fd);

i32 world_chunks_store_hashed(World* world, const char* world_storage_file);

i32 world_chunk_load_hashed(World_chunk* chunk, World_position position, const char* world_storage_file);

#endif
