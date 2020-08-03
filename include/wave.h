// wave.h

#ifndef _WAVE_H
#define _WAVE_H

struct Audio_source;

i32 load_wave_from_file(const char* filename, struct Audio_source* source);

#endif
