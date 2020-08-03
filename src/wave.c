// wave.c

#include "common.h"
#include "audio.h"
#include "wave.h"

struct RIFF_header {
  char chunk_id[4];  // 'R', 'I', 'F', 'F'
  i32 size;
  char wave_id[4];  // 'W', 'A', 'V', 'E'
  char format_chunk[4];
  i32 format_chunk_size;
  i16 format_type;
  i16 channel_count;
  i32 sample_rate;
  i32 data_rate;  // (sample rate * bits per sample * channels) / 8
  i16 data_block_size;  // (BitsPerSample * Channels) / 8
  i16 bits_per_sample;
  i32 data_chunk_header;
  i32 data_size;
} __attribute__((packed));

i32 load_wave_from_file(const char* filename, struct Audio_source* source) {
  (void)source;

  FILE* fp = fopen(filename, "rb");
  if (!fp) {
    fprintf(stderr, "Failed to open file '%s'\n", filename);
    return -1;
  }
  struct RIFF_header header = {0};

  fseek(fp, 0, SEEK_END);
  i32 file_size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  if (file_size < (i32)sizeof(struct RIFF_header)) {
    fprintf(stderr, "Invalid WAV file\n");
    fclose(fp);
    return -1;
  }

  i32 bytes_read = fread(&header, 1, sizeof(struct RIFF_header), fp);
  assert(bytes_read == sizeof(struct RIFF_header));

  printf(
    "RIFF header (file: '%s'):\n"
    "  chunk_id: %.4s\n"
    "  size: %i\n"
    "  wave_id: %.4s\n"
    "  format_chunk: %.4s\n"
    "  format_chunk_size: %i\n"
    "  format_type: %i\n"
    "  channel_count: %i\n"
    "  sample_rate: %i\n"
    "  data_rate: %i\n"
    "  data_block_size: %i\n"
    "  bits_per_sample: %i\n"
    "  data_chunk_header: %i\n"
    "  data_size: %i\n"
    ,
    filename,
    header.chunk_id,
    header.size,
    header.wave_id,
    header.format_chunk,
    header.format_chunk_size,
    header.format_type,
    header.channel_count,
    header.sample_rate,
    header.data_rate,
    header.data_block_size,
    header.bits_per_sample,
    header.data_chunk_header,
    header.data_size
  );

  fclose(fp);
  return 0;
}
