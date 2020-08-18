// world.c

#include <fcntl.h>
#include <unistd.h>

#include "game.h"
#include "world.h"

i32 world_chunk_store(struct World_chunk* chunk, const char* world_storage_file) {
  i32 fd = open(world_storage_file, O_WRONLY | O_CREAT, 0644);
  if (fd < 0) {
    fprintf(stderr, "Failed to open file '%s'\n", world_storage_file);
    return -1;
  }
  u32 chunk_address = sizeof(struct World_chunk) * chunk->chunk_index;
  lseek(fd, chunk_address, SEEK_SET);
  u32 bytes_written = write(fd, chunk, sizeof(struct World_chunk));
  if (bytes_written != sizeof(struct World_chunk)) {
    close(fd);
    fprintf(stderr, "Failed to write world chunk\n");
    return -1;
  }

  close(fd);
  return 0;
}

i32 world_chunk_load(struct World_chunk* chunk, u32 chunk_index, const char* world_storage_file) {
  FILE* fp = fopen(world_storage_file, "rb");

  if (!fp) {
    fprintf(stderr, "Failed to open file '%s'\n", world_storage_file);
    return -1;
  }

  fseek(fp, 0, SEEK_END);
  u32 size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  if (size < sizeof(struct World_chunk)) {
    fprintf(stderr, "Invalid world storage file '%s'\n", world_storage_file);
    fclose(fp);
    return -1;
  }
  u32 chunk_address = chunk_index * sizeof(struct World_chunk);
  if (chunk_address > size) {
    fprintf(stderr, "Failed to load chunk (id: %u)\n", chunk_index);
    fclose(fp);
    return -1;
  }
  fseek(fp, chunk_address, SEEK_SET);
  u32 num_read = fread(chunk, sizeof(struct World_chunk), 1, fp);
  if (num_read != 1) {
    fprintf(stderr, "Failed to read world chunk (id: %u)\n", chunk_index);
    fclose(fp);
    return -1;
  }

  fclose(fp);
  return 0;
}

