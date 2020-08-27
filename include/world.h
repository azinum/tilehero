// world.h

#ifndef _WORLD_H
#define _WORLD_H

#define MAX_ENTITY (256)

typedef struct World_chunk {
  vec3i position;
  u32 chunk_index;
  u32 entity_count;
  struct Tile_map tile_map;
  struct Entity entities[MAX_ENTITY];
} World_chunk;

#define NUM_WORLD_CHUNKS 4096
#define CHUNK_AREA_X 3
#define CHUNK_AREA_Y 3
#define NUM_LOADED_WORLD_CHUNKS (CHUNK_AREA_X * CHUNK_AREA_Y)

#define CHUNK_SIZE_IN_PIXELS_X (TILE_COUNT_X * TILE_SIZE)
#define CHUNK_SIZE_IN_PIXELS_Y (TILE_COUNT_Y * TILE_SIZE)

#define PIXEL_TO_WORLD_X_POSITION(PX) (i32)((PX / (CHUNK_SIZE_IN_PIXELS_X))
#define PIXEL_TO_WORLD_Y_POSITION(PX) (i32)((PX / (CHUNK_SIZE_IN_PIXELS_Y))

typedef struct World {
  vec3i current_origin;
  World_chunk chunks[NUM_LOADED_WORLD_CHUNKS];

  // NOTE(lucas): These are the entities in which we are currently processing
  Entity entities[MAX_ENTITY * NUM_LOADED_WORLD_CHUNKS];
  u32 entity_count;
} World;

void world_chunk_init(World_chunk* chunk, vec3i position);

void world_load_chunks_from_world_position(World* world, vec3i position);

i32 world_add_entity_to_chunk(World_chunk* chunk, Entity* e);

i32 world_transfer_entities_to_chunks(World* world);

i32 world_chunk_store(World_chunk* chunk, i32 fd);

i32 world_chunks_store_hashed(World* world, const char* world_storage_file);

i32 world_chunk_load_hashed(World_chunk* chunk, vec3i position, const char* world_storage_file);

#endif
