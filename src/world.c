// world.c

#include <fcntl.h>
#include <unistd.h>

#include "game.h"
#include "world.h"

inline i32 hash_world_position(vec3i pos);

i32 hash_world_position(vec3i pos) {
  i32 result = abs(((pos.x * 137) + (pos.y * 149) + (pos.z * 163))) % NUM_WORLD_CHUNKS;

  return result;
}

void world_chunk_init(World_chunk* chunk, vec3i position) {
  chunk->position = position;
  chunk->chunk_index = 0;
  chunk->entity_count = 0;
  tilemap_init(&chunk->tile_map, TILE_COUNT_X, TILE_COUNT_Y);
}

void world_load_chunks_from_world_position(World* world, vec3i position) {
  world->current_origin = position;
  u32 index = 0;
  for (i32 y = -1; y <= 1; y++) {
    for (i32 x = -1; x <= 1; x++) {
      vec3i p = VEC3I(position.x + x, position.y + y, position.z);
      World_chunk* chunk = &world->chunks[index++];
      world_chunk_load_hashed(chunk, p, WORLD_STORAGE_FILE);
      assert(chunk);

      for (u32 i = 0; i < chunk->entity_count; i++) {
        game_copy_add_entity(chunk->entities[i]);
      }
    }
  }
}

i32 world_add_entity_to_chunk(World_chunk* chunk, Entity* e) {
  if (chunk->entity_count >= MAX_ENTITY) {
    fprintf(stderr, "Failed to add entity to world chunk\n");
    return -1;
  }
  chunk->entities[chunk->entity_count++] = *e;
  return 0;
}

i32 world_transfer_entities_to_chunks(World* world) {
  for (u32 chunk_index = 0; chunk_index < NUM_LOADED_WORLD_CHUNKS; chunk_index++) {
    World_chunk* chunk = &world->chunks[chunk_index];
    chunk->entity_count = 0;
  }

  for (u32 entity_index = 0; entity_index < world->entity_count; entity_index++) {
    Entity* e = &world->entities[entity_index];
    assert(e);

    vec3i world_position = VEC3I((i32)(e->x_tile / TILE_COUNT_X), (i32)(e->y_tile / TILE_COUNT_Y), 0);
    vec3i origin = world->chunks[0].position;
    vec3i offset = VEC3I_OFFSET(world_position, origin);
    i32 chunk_index = offset.x + (CHUNK_AREA_X * offset.y);
    if (chunk_index >= 0) {
      World_chunk* chunk = &world->chunks[chunk_index];
      world_add_entity_to_chunk(chunk, e);
    }
    else {
      fprintf(stderr, "Failed to transfer entity to chunk index %i\n", chunk_index);
    }
  }
  world->entity_count = 0;
  return 0;
}

i32 world_chunk_store(World_chunk* chunk, i32 fd) {
  assert(fd >= 0);

  i32 hash_value = hash_world_position(chunk->position);
  i32 address = hash_value * sizeof(World_chunk);
  lseek(fd, address, SEEK_SET);

  u32 bytes_written = write(fd, chunk, sizeof(World_chunk));
  if (bytes_written != sizeof(World_chunk)) {
    fprintf(stderr, "Failed to write world chunk to storage file (location: %i, %i, %i)\n", chunk->position.x, chunk->position.y, chunk->position.z);
    return -1;
  }
  return 0;
}

i32 world_chunks_store_hashed(World* world, const char* world_storage_file) {
  i32 fd = open(world_storage_file, O_RDWR | O_CREAT, 0644);
  if (fd < 0) {
    fprintf(stderr, "Failed to open file '%s'\n", world_storage_file);
    return -1;
  }

  for (u32 i = 0; i < NUM_LOADED_WORLD_CHUNKS; i++) {
    World_chunk* chunk = &world->chunks[i];
    assert(chunk);
    world_chunk_store(chunk, fd);
  }

  close(fd);
  return 0;
}

i32 world_chunk_load_hashed(World_chunk* chunk, vec3i pos, const char* world_storage_file) {
  i32 fd = open(world_storage_file, O_RDWR | O_CREAT, 0644);
  if (fd < 0) {
    fprintf(stderr, "Failed to open file '%s'\n", world_storage_file);
    return -1;
  }

  i32 hash_value = hash_world_position(pos);
  i32 address = hash_value * sizeof(struct World_chunk);
  lseek(fd, address, SEEK_SET);

  World_chunk temp_chunk = {0};
  u32 bytes_read = 0;
  u32 bytes_written = 0;

  do {
    bytes_read = read(fd, &temp_chunk, sizeof(World_chunk));
    if (bytes_read != sizeof(World_chunk)) {
      world_chunk_init(&temp_chunk, pos);
      lseek(fd, address, SEEK_SET);
      bytes_written = write(fd, &temp_chunk, sizeof(World_chunk));
      (void)bytes_written;
    }
    else if (!(VEC3I_EQUAL(pos, temp_chunk.position))) {
      world_chunk_init(&temp_chunk, pos);
      lseek(fd, address, SEEK_SET);
      bytes_written = write(fd, &temp_chunk, sizeof(World_chunk));
      (void)bytes_written;
    }
    break;
  } while (1);

  *chunk = temp_chunk;
  close(fd);
  return 0;
}
