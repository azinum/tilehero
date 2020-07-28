// texture_loader.c

#include <png.h>

#include "common.h"
#include "renderer_common.h"
#include "image_loader.h"
#include "texture_loader.h"

// TODO(lucas): Figure out a way to encapsulate this better.
u32 load_texture(const char* path) {
  u32 texture_id = 0;
  Image image;
  if (load_image(path, &image) != 0) {
    fprintf(stderr, "Failed to load png texture\n");
    return 0;
  }
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);  // GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.pixel_buffer);
  glBindTexture(GL_TEXTURE_2D, 0);

  free(image.pixel_buffer);
  return texture_id;
}
