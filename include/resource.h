// resource.h

#ifndef _RESOURCE_H
#define _RESOURCE_H

enum Texture_type {
  TEXTURE_SPRITES,
  MAX_TEXTURE,
};

struct Texture {
  i16 w;
  i16 h;
  u32 id;
};

extern struct Texture textures[MAX_TEXTURE];

struct Texture load_texture_from_file(const char* path);

void resources_load();

#endif
