// audio.h

#ifndef _AUDIO_H
#define _AUDIO_H

#define SAMPLE_RATE (44100)
#define FRAMES_PER_BUFFER (512)

struct Audio_source {
  float* sample_buffer;
  u32 sample_count;
  u32 sample_rate;
  i16 channel_count;
};

typedef void (*callback_func)();

i32 audio_engine_init(i32 sample_rate, i32 frames_per_buffer, callback_func callback);

void audio_play_once_on_channel(u32 sound_id, u32 channel, float amp);

void audio_play_once(u32 sound_id, float amp);

#endif
