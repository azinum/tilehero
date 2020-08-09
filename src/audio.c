// audio.c

#include <portaudio.h>

#include "common.h"
#include "resource.h"
#include "audio.h"

#define MAX_SOUNDS_PLAYING (32)
#define MASTER_VOLUME (1.0f)

enum Channel_type {
  CHANNEL_MUSIC,
  CHANNEL_AMBIENCE,
  MAX_CHANNEL,
};

struct Sound_state {
  i32 id;
  u32 sample_index;
  float amp;
};

typedef struct Audio_engine {
  i32 sample_rate;
  i32 frames_per_buffer;
  i32 tick;
  struct Sound_state sounds[MAX_SOUNDS_PLAYING];
  u32 sound_count;
  float master_volume;
  struct Sound_state channels[MAX_CHANNEL];
  PaStream* stream;
  PaStreamParameters in_port, out_port;
} Audio_engine;

static Audio_engine audio_engine;

static i32 open_stream();
static i32 stereo_callback(const void* in_buff, void* out_buff, unsigned long frames_per_buffer, const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags flags, void* user_data);

i32 open_stream() {
  PaError err = Pa_OpenStream(
    &audio_engine.stream,
    NULL,
    &audio_engine.out_port,
    audio_engine.sample_rate,
    audio_engine.frames_per_buffer,
    0,
    stereo_callback,
    NULL
  );
  if (err != paNoError) {
    Pa_Terminate();
    fprintf(stderr, "[PortAudio error]: %s\n", Pa_GetErrorText(err));
    return -1;
  }
  Pa_StartStream(audio_engine.stream);
  return 0;
}

i32 stereo_callback(const void* in_buff, void* out_buff, unsigned long frames_per_buffer, const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags flags, void* user_data) {
  (void)in_buff; (void)time_info; (void)flags; (void)user_data;
  float* out = (float*)out_buff;

  for (i32 buffer_index = 0; buffer_index < (i32)frames_per_buffer; buffer_index++) {
    float l_frame = 0, r_frame = 0;

    for (u32 i = 0; i < MAX_CHANNEL; i++) {
      struct Sound_state* sound = &audio_engine.channels[i];
      const struct Audio_source* source = &sounds[sound->id];
      if (sound->sample_index < source->sample_count) {
        if (source->channel_count == 2) {
          l_frame += sound->amp * source->sample_buffer[sound->sample_index++];
          r_frame += sound->amp * source->sample_buffer[sound->sample_index++];
        }
        else {
          float frame = sound->amp * source->sample_buffer[sound->sample_index++];
          l_frame += frame;
          r_frame += frame;
        }
      }
    }

    for (u32 i = 0; i < audio_engine.sound_count; i++) {
      struct Sound_state* sound = &audio_engine.sounds[i];
      const struct Audio_source* source = &sounds[sound->id];
      if (sound->sample_index < source->sample_count) {
        if (source->channel_count == 2) {
          l_frame += sound->amp * source->sample_buffer[sound->sample_index++];
          r_frame += sound->amp * source->sample_buffer[sound->sample_index++];
        }
        else {
          float frame = sound->amp * source->sample_buffer[sound->sample_index++];
          l_frame += frame;
          r_frame += frame;
        }
      }
    }
    *out++ = audio_engine.master_volume * l_frame;
    *out++ = audio_engine.master_volume * r_frame;
    audio_engine.tick++;
  }

  for (u32 i = 0; i < audio_engine.sound_count; i++) {
    struct Sound_state* sound = &audio_engine.sounds[i];
    const struct Audio_source* source = &sounds[sound->id];
    if (sound->sample_index >= source->sample_count) {
      audio_engine.sounds[i] = audio_engine.sounds[--audio_engine.sound_count];
    }
  }
  return paContinue;
}

i32 audio_engine_init(i32 sample_rate, i32 frames_per_buffer, callback_func callback) {
  PaError err = Pa_Initialize();
  if (err != paNoError) {
    Pa_Terminate();
    fprintf(stderr, "[portaudio error]: %s\n", Pa_GetErrorText(err));
    return -1;
  }
  audio_engine.sample_rate = sample_rate;
  audio_engine.frames_per_buffer = frames_per_buffer;
  audio_engine.tick = 0;
  audio_engine.sound_count = 0;
  audio_engine.master_volume = MASTER_VOLUME;
  memset(audio_engine.channels, 0, sizeof(struct Sound_state) * MAX_CHANNEL);

  i32 output_device = Pa_GetDefaultOutputDevice();
  audio_engine.out_port.device = output_device;
  audio_engine.out_port.channelCount = 2;
  audio_engine.out_port.sampleFormat = paFloat32;
  audio_engine.out_port.suggestedLatency = Pa_GetDeviceInfo(audio_engine.out_port.device)->defaultHighOutputLatency;
  audio_engine.out_port.hostApiSpecificStreamInfo = NULL;
  if (open_stream() != 0) {
    return -1;
  }
  if (callback)
    callback();
  return 0;
}

void audio_play_once_on_channel(i32 sound_id, u32 channel, float amp) {
  assert(channel < MAX_CHANNEL);
  assert(sound_id >= 0 && sound_id < MAX_SOUND);

  struct Audio_source* source = &sounds[sound_id];
  if (!source->sample_buffer) {
    resource_load_sound(sound_id);
  }

  struct Sound_state sound = {
    .id = sound_id,
    .sample_index = 0,
    .amp = amp
  };
  audio_engine.channels[channel] = sound;
}

// NOTE(lucas): We are expecting a valid sound id here.
void audio_play_once(i32 sound_id, float amp) {
  if (audio_engine.sound_count >= MAX_SOUNDS_PLAYING) {
    fprintf(stderr, "[Warning]: Too many sounds playing at once!\n");
    return;
  }
  assert(sound_id >= 0 && sound_id < MAX_SOUND);

  struct Audio_source* source = &sounds[sound_id];
  if (!source->sample_buffer) {
    resource_load_sound(sound_id);
  }

  struct Sound_state sound = {
    .id = sound_id,
    .sample_index = 0,
    .amp = amp
  };
  audio_engine.sounds[audio_engine.sound_count++] = sound;
}
