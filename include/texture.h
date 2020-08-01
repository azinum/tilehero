// texture.h

#ifndef _TEXTURE_H
#define _TEXTURE_H

struct Image;

// NOTE(lucas): Unused.
struct Texture {
  u32 w;
  u32 h;
  u32 id;
};

u32 load_texture(const char* path);

u32 load_texture_from_image(struct Image* image);

#endif
