// level.c

#include <fcntl.h>
#include <unistd.h>

#include "game.h"
#include "editor.h" // Probably want to abstract this out; we don't need to couple the editor to the level loader like this.
#include "level.h"

void save_state_init(Save_state* state) {
  state->score = 0;
  state->level = 0;
}

i32 save_state_store(Save_state* state) {
  i32 fd = open(SAVE_STATE_FILE, O_RDWR | O_CREAT, 0644);
  if (fd < 0) {
    fprintf(stderr, "Failed to open file '%s'\n", SAVE_STATE_FILE);
    return -1;
  }
  u32 bytes_written = write(fd, state, sizeof(Save_state));
  if (bytes_written != sizeof(Save_state)) {
    fprintf(stderr, "Failed to write to file '%s'\n", SAVE_STATE_FILE);
    close(fd);
    return -1;
  }
  fprintf(log_file, "Stored save state to '%s'\n", SAVE_STATE_FILE);
  close(fd);
  return 0;
}

i32 save_state_load(Save_state* state) {
  i32 fd = open(SAVE_STATE_FILE, O_RDWR | O_CREAT, 0644);
  if (fd < 0) {
    fprintf(stderr, "Failed to open file '%s'\n", SAVE_STATE_FILE);
    return -1;
  }
  u32 bytes_read = read(fd, state, sizeof(Save_state));

  if (bytes_read != sizeof(Save_state)) {
    save_state_init(state);
    u32 bytes_written = write(fd, state, sizeof(Save_state));
    if (bytes_written != sizeof(Save_state)) {
      fprintf(stderr, "Failed to store save state on '%s'\n", SAVE_STATE_FILE);
      close(fd);
      return -1;
    }
    else {
      fprintf(log_file, "Initialized save state on file '%s'\n", SAVE_STATE_FILE);
    }
  }
  fprintf(log_file, "Loaded save state from '%s'\n", SAVE_STATE_FILE);
  close(fd);
  return 0;
}

void level_init(Level* level) {
  level->index = 0;
  level->entity_count = 0;
  tilemap_init(&level->tile_map, TILE_COUNT_X, TILE_COUNT_Y);
}

i32 level_store(World* world, u32 index) {
  i32 fd = open(WORLD_STORAGE_FILE, O_RDWR | O_CREAT, 0644);
  if (fd < 0) {
    fprintf(stderr, "Failed to open file '%s'\n", WORLD_STORAGE_FILE);
    return -1;
  }

  Level* level = &world->level;
  level->index = index;
  level->entity_count = 0;

  for (u32 i = 0; i < world->entity_count; i++) {
    Entity* e = &world->entities[i];
    Entity_def def;
    if (entity_to_def(e, &def) == NO_ERR) {
      level->entities[level->entity_count++] = def;
    }
  }

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

i32 level_load(World* world, u32 index) {
  i32 fd = open(WORLD_STORAGE_FILE, O_RDWR | O_CREAT, 0644);
  if (fd < 0) {
    fprintf(stderr, "Failed to open file '%s'\n", WORLD_STORAGE_FILE);
    return -1;
  }
  u32 address = sizeof(Level) * index;
  lseek(fd, address, SEEK_SET);

  Level* level = &world->level;
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
  world->entity_count = 0;
  for (u32 i = 0; i < level->entity_count; i++) {
    Entity_def* def = &level->entities[i];
    editor_place_entity_from_def(def);
  }

  close(fd);
  return 0;
}
