// audio.c

#include <portaudio.h>

#include "common.h"
#include "audio.h"

typedef struct Audio_engine {
  i32 sample_rate;
  i32 frames_per_buffer;
  i32 tick;
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

  for (i32 i = 0; i < (i32)frames_per_buffer; i++) {
    float frame = 0.1f * (sin(audio_engine.tick * 220 * PI32 * 2 / audio_engine.sample_rate));
    *out++ = frame;
    *out++ = frame;
    audio_engine.tick++;
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

  i32 output_device = Pa_GetDefaultOutputDevice();
  audio_engine.out_port.device = output_device;
  audio_engine.out_port.channelCount = 2;
  audio_engine.out_port.sampleFormat = paFloat32;
  audio_engine.out_port.suggestedLatency = Pa_GetDeviceInfo(audio_engine.out_port.device)->defaultLowOutputLatency;
  audio_engine.out_port.hostApiSpecificStreamInfo = NULL;
  if (open_stream() != 0) {
    return -1;
  }
  if (callback)
    callback();
  return 0;
}
