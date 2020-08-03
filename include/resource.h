// resource.h

#ifndef _RESOURCE_H
#define _RESOURCE_H

enum Texture_type {
  TEXTURE_SPRITES,
  MAX_TEXTURE,
};

enum Sound_type {
  SOUND_HIT_HURT,
  SOUND_RANDOM_1,
  SOUND_RANDOM_2,
  MAX_SOUND,
};

struct Texture {
  i16 w;
  i16 h;
  u32 id;
};

struct Sound {
  u32 id;
};

extern struct Texture textures[MAX_TEXTURE];
extern struct Sound sounds[MAX_SOUND];

struct Texture load_texture_from_file(const char* path);

void resources_load();

#endif
