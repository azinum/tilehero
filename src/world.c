// world.c

#include <fcntl.h>
#include <unistd.h>

#include "game.h"
#include "world.h"

inline i32 hash_position(World_position pos);

i32 hash_position(World_position pos) {
  i32 result = 0;

  result = ((pos.x * 137) + (pos.y * 149) + (pos.z * 163)) % NUM_WORLD_CHUNKS;

  return result;
}

void world_chunk_init(struct World_chunk* chunk, World_position pos) {
  chunk->position = pos;
  chunk->chunk_index = 0;
  chunk->entity_count = 0;
}

i32 world_chunk_store_hashed(struct World_chunk* chunk, const char* world_storage_file) {
  i32 fd = open(world_storage_file, O_RDWR | O_CREAT, 0644);
  if (fd < 0) {
    fprintf(stderr, "Failed to open file '%s'\n", world_storage_file);
    return -1;
  }

  i32 hash_value = hash_position(chunk->position);
  i32 address = hash_value * sizeof(struct World_chunk);
  lseek(fd, address, SEEK_SET);

  // TODO(lucas): Read at this address to check for collisions before writing here!
  u32 bytes_written = write(fd, chunk, sizeof(struct World_chunk));
  if (bytes_written != sizeof(struct World_chunk)) {
    fprintf(stderr, "Failed to write changes to world storage file '%s'\n", world_storage_file);
    close(fd);
    return -1;
  }

  close(fd);
  return 0;
}

i32 world_chunk_load_hashed(struct World_chunk* chunk, World_position pos, const char* world_storage_file) {
  i32 fd = open(world_storage_file, O_RDONLY);
  if (fd < 0) {
    fprintf(stderr, "Failed to open file '%s'\n", world_storage_file);
    return -1;
  }

  i32 hash_value = hash_position(pos);
  i32 address = hash_value * sizeof(struct World_chunk);
  lseek(fd, address, SEEK_SET);

  struct World_chunk temp_chunk;
  u32 bytes_read = read(fd, &temp_chunk, sizeof(struct World_chunk));
  if (bytes_read != sizeof(struct World_chunk)) {
    fprintf(stderr, "Failed to read chunk (%i, %i, %i) in world storage file '%s'\n", pos.x, pos.y, pos.z, world_storage_file);
    close(fd);
    return -1;
  }
  *chunk = temp_chunk;
  close(fd);
  return 0;
}
