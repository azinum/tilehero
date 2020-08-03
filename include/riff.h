// riff.h

#ifndef _RIFF_H
#define _RIFF_H

struct Audio_source;

i32 load_wav_from_file(const char* filename, struct Audio_source* source);

#endif
