// wave.h

#ifndef _WAVE_H
#define _WAVE_H

struct Audio_source {
  float* sample_buffer;
  u32 sample_count;
  u32 sample_rate;
};

i32 load_wave_from_file(const char* filename, struct Audio_source* source);

#endif
