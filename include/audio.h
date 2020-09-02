// audio.h

#ifndef _AUDIO_H
#define _AUDIO_H

#define SAMPLE_RATE (44100)
#define FRAMES_PER_BUFFER (512)
#define MAX_ACTIVE_SOUNDS (128)

enum Channel_type {
  CHANNEL_MUSIC,
  CHANNEL_AMBIENCE,
  MAX_CHANNEL,
};

struct Audio_source {
  float* sample_buffer;
  u32 sample_count;
  u32 sample_rate;
  i16 channel_count;
};

struct Sound_state {
  u32 id;
  u32 sample_index;
  float amp;
};

typedef struct Audio_engine {
  i32 sample_rate;
  i32 frames_per_buffer;
  i32 tick;
  struct Sound_state sounds[MAX_ACTIVE_SOUNDS];
  u32 sound_count;
  float master_volume;
  float pan;
  u8 muted;
} Audio_engine;

extern struct Audio_engine audio_engine;

typedef void (*callback_func)();

i32 audio_engine_init(i32 sample_rate, i32 frames_per_buffer, callback_func callback);

void audio_play_once_on_channel(u32 sound_id, u32 channel, float amp);

void audio_play_once(u32 sound_id, float amp);

#endif
