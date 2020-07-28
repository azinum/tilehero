// image_loader.h

#ifndef _IMAGE_LOADER_H
#define _IMAGE_LOADER_H

typedef struct Image {
  u8* pixel_buffer;
  i16 width;
  i16 height;
  i16 depth;
  i16 pitch;
} Image;

i32 load_image(const char* path, struct Image* image);

#endif
