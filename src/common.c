// common.c

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "common.h"

FILE* log_file;

char* read_entire_file(const char* path) {
  FILE* fp = fopen(path, "rb");
  if (!fp) {
    fprintf(stderr, "Failed to open file '%s'\n", path);
    return NULL;
  }
  char* result = NULL;
  int buffer_size = 0;
  int read_size = 0;

  fseek(fp, 0, SEEK_END);
  buffer_size = ftell(fp);
  rewind(fp);

  result = (char*)malloc(sizeof(char) * buffer_size + 1);

  read_size = fread(result, sizeof(char), buffer_size, fp);
  result[read_size] = 0;

  if (buffer_size != read_size) {
    assert(0);
    free(result);
    result = NULL;
  }

  fclose(fp);
  return result;
}

float lerp(float v0, float v1, float t) {
  return (1 - t) * v0 + t * v1;
}

float lerp2(float v0, float v1, float t, float delta) {
  if (fabs(v0 - v1) < delta) {
    return v1;
  }
  return (1.0f - t) * v0 + t * v1;
}
