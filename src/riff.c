// riff.c

#include "common.h"
#include "audio.h"
#include "riff.h"

#define FORMAT_PCM 0x1  // Uncompressed format

struct RIFF_header {
  char chunk_id[4];
  i32 size;
  char wave_id[4];
  char format_chunk[4];
  i32 format_chunk_size;
  i16 format_type;
  i16 channel_count;
  i32 sample_rate;
  i32 data_rate;  // (sample rate * bits per sample * channels) / 8
  i16 data_block_size;  // (bits per sample * channels) / 8 => bytes per frame / sample
  i16 bits_per_sample;
  char data_chunk_header[4];
  i32 data_size;
} __attribute__((packed));

static void print_riff_header(const char* filename, struct RIFF_header* header);

void print_riff_header(const char* filename, struct RIFF_header* header) {
  printf(
    "RIFF header(%s):\n"
    "  chunk_id: %.4s\n"
    "  size: %i\n"
    "  wave_id: %.4s\n"
    "  format_chunk: %.4s\n"
    "  format_chunk_size: %i\n"
    "  format_type: 0x%x\n"
    "  channel_count: %i\n"
    "  sample_rate: %i\n"
    "  data_rate: %i\n"
    "  data_block_size: %i\n"
    "  bits_per_sample: %i\n"
    "  data_chunk_header: %.4s\n"
    "  data_size: %i\n"
    ,
    filename,
    header->chunk_id,
    header->size,
    header->wave_id,
    header->format_chunk,
    header->format_chunk_size,
    header->format_type,
    header->channel_count,
    header->sample_rate,
    header->data_rate,
    header->data_block_size,
    header->bits_per_sample,
    header->data_chunk_header,
    header->data_size
  );
}

inline void i16_to_f32(float* out, const i16* in, i32 sample_count) {
  assert(out != NULL && in != NULL);

  for (i32 i = 0; i < sample_count; i++) {
    *out++ = in[i] / 32768.0f;
  }
}

i32 load_wav_from_file(const char* filename, struct Audio_source* source) {
  i32 status = 0;
  FILE* fp = fopen(filename, "rb");
  if (!fp) {
    fprintf(stderr, "Failed to open file '%s'\n", filename);
    return -1;
  }
  struct RIFF_header header = {0};

  fseek(fp, 0, SEEK_END);
  i32 file_size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  (void)file_size;

  i32 bytes_read = fread(&header, 1, sizeof(struct RIFF_header), fp);
  if (bytes_read < (i32)sizeof(struct RIFF_header)) {
    fprintf(stderr, "Invalid WAV file\n");
    status = -1;
    goto done;
  }
#if 0
  print_riff_header(filename, &header);
#else
  (void)print_riff_header;
#endif
  if (header.format_type != FORMAT_PCM) {
    fprintf(stderr, "Format not supported (0x%x)\n", header.format_type);
    status = -1;
    goto done;
  }

  const i32 sample_count = header.channel_count * (header.data_size / header.data_block_size);
  void* sample_data = malloc(header.data_size);
  i32 sample_data_bytes_read = fread(sample_data, 1, header.data_size, fp);

  assert(sample_data_bytes_read == header.data_size);

  source->sample_buffer = malloc(sample_count * sizeof(float));
  i16_to_f32(source->sample_buffer, (i16*)sample_data, sample_count);
  source->sample_count = sample_count;
  source->sample_rate = header.sample_rate;
  source->channel_count = header.channel_count;
  free(sample_data);

done:
  fclose(fp);
  return status;
}
