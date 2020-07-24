// audio_engine.h

#ifndef _AUDIO_ENGINE_H
#define _AUDIO_ENGINE_H

#define SAMPLE_RATE (44100)
#define FRAMES_PER_BUFFER (256)

typedef void (*callback_func)();

i32 audio_engine_init(i32 sample_rate, i32 frames_per_buffer, callback_func callback);

#endif
