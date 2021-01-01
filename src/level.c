// level.c

#include <fcntl.h>
#include <unistd.h>

#include "game.h"
#include "level.h"

void level_init(Level* level) {
  level->index = 0;
  level->entity_count = 0;
  tilemap_init(&level->tile_map, TILE_COUNT_X, TILE_COUNT_Y);
}

i32 level_store(Level* level, u32 index) {
  i32 fd = open(WORLD_STORAGE_FILE, O_RDWR | O_CREAT, 0644);
  if (fd < 0) {
    fprintf(stderr, "Failed to open file '%s'\n", WORLD_STORAGE_FILE);
    return -1;
  }

  level->index = index;

  u32 address = sizeof(Level) * index;
  lseek(fd, address, SEEK_SET);
  u32 bytes_written = write(fd, level, sizeof(Level));
  if (bytes_written != sizeof(Level)) {
    fprintf(stderr, "Failed to write to file '%s'\n", WORLD_STORAGE_FILE);
    close(fd);
    return -1;
  }
  fprintf(log_file, "Saved level '%i' to '%s'\n", index, WORLD_STORAGE_FILE);

  close(fd);
  return 0;
}

i32 level_load(Level* level, u32 index) {
  i32 fd = open(WORLD_STORAGE_FILE, O_RDWR | O_CREAT, 0644);
  if (fd < 0) {
    fprintf(stderr, "Failed to open file '%s'\n", WORLD_STORAGE_FILE);
    return -1;
  }
  u32 address = sizeof(Level) * index;
  lseek(fd, address, SEEK_SET);
  
  Level level_read = {0};
  level_init(&level_read);
  level_read.index = index;

  u32 bytes_read = read(fd, &level_read, sizeof(Level));
  u32 bytes_written = 0;
  if (bytes_read != sizeof(Level)) {
    bytes_written = write(fd, &level_read, sizeof(Level));
    if (bytes_written != sizeof(Level)) {
      fprintf(stderr, "Failed to write level (%i) to storage file\n", index);
      close(fd);
      return -1;
    }
  }

  *level = level_read;

  close(fd);
  return 0;
}
