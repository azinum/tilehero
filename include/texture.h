// texture.h

#ifndef _TEXTURE_H
#define _TEXTURE_H

struct Image;

u32 load_texture(const char* path);

u32 load_texture_from_image(struct Image* image);

#endif
