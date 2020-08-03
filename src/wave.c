// wave.c

#include "common.h"
#include "wave.h"

i32 load_wave_from_file(const char* filename, struct Audio_source* source) {
  (void)source;
  FILE* fp = fopen(filename, "rb");
  if (!fp) {
    fprintf(stderr, "Failed to open file '%s'\n", filename);
    return -1;
  }
  fclose(fp);
  return 0;
}
