// audio.c

#include <portaudio.h>

#include "common.h"
#include "resource.h"
#include "config.h"
#include "audio.h"

Audio_engine audio_engine;

static PaStream* stream;
static PaStreamParameters out_port;

static i32 open_stream();
static i32 stereo_callback(const void* in_buff, void* out_buff, unsigned long frames_per_buffer, const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags flags, void* user_data);

i32 open_stream() {
  PaError err = Pa_OpenStream(
    &stream,
    NULL,
    &out_port,
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
  Pa_StartStream(stream);
  return 0;
}

i32 stereo_callback(const void* in_buff, void* out_buff, unsigned long frames_per_buffer, const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags flags, void* user_data) {
  (void)in_buff; (void)time_info; (void)flags; (void)user_data;
  float* out = (float*)out_buff;

// TODO(lucas): Compare SSE vs no SSE!
#if USE_SSE
  __m128* dest = (__m128*)out;
  __m128 zero = _mm_set1_ps(0.0f);
  (void)zero;
  __m128 master_volume = _mm_set1_ps(audio_engine.master_volume);
  u32 chunk_count = frames_per_buffer / 4;
  for (u32 chunk_index = 0; chunk_index < chunk_count; chunk_index++) {
    audio_engine.tick++;
    __m128 frame0 = _mm_set1_ps(0.0f);
    __m128 frame1 = _mm_set1_ps(0.0f);
    for (u32 i = 0; i < MAX_CHANNEL + audio_engine.sound_count; i++) {
      struct Sound_state* sound = &audio_engine.sounds[i];
      const struct Audio_source* source = &sounds[sound->id];
      if (!source->sample_buffer) {
        continue;
      }
      u32 source_chunk_count = source->sample_count / (4 * source->channel_count);
      u32 sound_state_chunk_count = sound->sample_index / (4 * source->channel_count);
      if (sound_state_chunk_count < source_chunk_count) {
        __m128 amp = _mm_set1_ps(sound->amp);
        __m128 sample_value0 = _mm_setr_ps(
          source->sample_buffer[(sound->sample_index) + 0],
          source->sample_buffer[(sound->sample_index) + 1],
          source->sample_buffer[(sound->sample_index) + 2],
          source->sample_buffer[(sound->sample_index) + 3]
        );
        sample_value0 = _mm_mul_ps(sample_value0, amp);

        if (source->channel_count == 2) {
          __m128 sample_value1 = _mm_setr_ps(
            source->sample_buffer[(sound->sample_index) + 4],
            source->sample_buffer[(sound->sample_index) + 5],
            source->sample_buffer[(sound->sample_index) + 6],
            source->sample_buffer[(sound->sample_index) + 7]
          );
          sample_value1 = _mm_mul_ps(sample_value1, amp);
          frame0 = _mm_add_ps(frame0, sample_value0);
          frame1 = _mm_add_ps(frame1, sample_value1);
        }
        else {
          frame0 = _mm_add_ps(frame0, sample_value0);
          frame1 = _mm_add_ps(frame1, sample_value0);
        }
        sound->sample_index += (4 * source->channel_count);
      }
    }
    frame0 = _mm_mul_ps(frame0, master_volume);
    frame1 = _mm_mul_ps(frame1, master_volume);
    _mm_store_ps((float*)dest++, frame0);
    _mm_store_ps((float*)dest++, frame1);
  }

#else
  for (i32 buffer_index = 0; buffer_index < (i32)frames_per_buffer; buffer_index++) {
    float l_frame = 0, r_frame = 0;

    for (u32 i = 0; i < MAX_CHANNEL + audio_engine.sound_count; i++) {
      struct Sound_state* sound = &audio_engine.sounds[i];
      const struct Audio_source* source = &sounds[sound->id];
      if (!source->sample_buffer) {
        continue;
      }
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
    l_frame *= audio_engine.master_volume;
    r_frame *= audio_engine.master_volume;
    *out++ = l_frame;
    *out++ = r_frame;
    audio_engine.tick++;
  }
#endif  // No SSE

  for (u32 i = MAX_CHANNEL; i < audio_engine.sound_count; i++) {
    struct Sound_state* sound = &audio_engine.sounds[i];
    const struct Audio_source* source = &sounds[sound->id];
#if USE_SSE
    u32 chunk_size = (4 * source->channel_count);
    if (sound->sample_index + chunk_size >= source->sample_count) {
      audio_engine.sounds[i] = audio_engine.sounds[MAX_CHANNEL + (--audio_engine.sound_count)];
    }
#else
    if (sound->sample_index >= source->sample_count) {
      audio_engine.sounds[i] = audio_engine.sounds[MAX_CHANNEL + (--audio_engine.sound_count)];
    }
#endif
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
  memset(audio_engine.sounds, 0, sizeof(struct Sound_state) * MAX_CHANNEL);

  i32 output_device = Pa_GetDefaultOutputDevice();
  out_port.device = output_device;
  out_port.channelCount = 2;
  out_port.sampleFormat = paFloat32;
  out_port.suggestedLatency = Pa_GetDeviceInfo(out_port.device)->defaultHighOutputLatency;
  out_port.hostApiSpecificStreamInfo = NULL;
  if (open_stream() != 0) {
    return -1;
  }
  if (callback) {
    callback();
  }
  Pa_CloseStream(stream);
  Pa_Terminate();
  return 0;
}

// NOTE(lucas): There are some slots that are reserved just for channels where you would play things like
// music, ambience, voice-overs e.t.c. This allows for the ability to play and pause the channels at any given moment.
void audio_play_once_on_channel(u32 sound_id, u32 channel, float amp) {
  assert(channel < MAX_CHANNEL);
  assert(sound_id < MAX_SOUND);

  struct Audio_source* source = &sounds[sound_id];
  if (!source->sample_buffer) {
    resource_load_sound(sound_id);
  }

  struct Sound_state sound = {
    .id = sound_id,
    .sample_index = 0,
    .amp = amp
  };
  audio_engine.sounds[channel] = sound;
}

// NOTE(lucas): We are expecting a valid sound id here.
void audio_play_once(u32 sound_id, float amp) {
  if ((audio_engine.sound_count + MAX_CHANNEL) >= MAX_ACTIVE_SOUNDS) {
    fprintf(stderr, "[Warning]: Too many sounds playing at once!\n");
    return;
  }
  assert(sound_id < MAX_SOUND);

  struct Audio_source* source = &sounds[sound_id];
  if (!source->sample_buffer) {
    resource_load_sound(sound_id);
  }

  struct Sound_state sound = {
    .id = sound_id,
    .sample_index = 0,
    .amp = amp
  };
  audio_engine.sounds[MAX_CHANNEL + audio_engine.sound_count] = sound;
  audio_engine.sound_count++;
}
