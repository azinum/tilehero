// resource.h

#ifndef _RESOURCE_H
#define _RESOURCE_H

#include "audio.h"

enum Texture_type {
  TEXTURE_SPRITES,
  TEXTURE_FONT,
  MAX_TEXTURE,
};

enum Sound_type {
  SOUND_RANDOM_1,
  SOUND_GOOD_MORNING,
  SOUND_HIT,
  SOUND_0F,

  SOUND_SONG_METAKING,
  MAX_SOUND,
};

struct Texture {
  u32 id;
  i16 w;
  i16 h;
};

extern struct Texture textures[];
extern struct Audio_source sounds[];

struct Texture load_texture_from_file(const char* path);

void resources_load();

void resource_load_sound(i32 sound_id);

void resources_unload();

#endif
