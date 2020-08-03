// resource.c

#include <png.h>

#include "common.h"
#include "image_loader.h"
#include "renderer_common.h"
#include "resource.h"

#define TEXTURE_PATH "resource/texture"
#define TEXTURE_EXT "png"

struct Texture textures[MAX_TEXTURE];

static const char* texture_filenames[MAX_TEXTURE] = {
  "spritesheet",
};

static u32 load_texture_from_image(struct Image* image);

u32 load_texture_from_image(struct Image* image) {
  u32 texture_id = 0;

  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixel_buffer);
  glBindTexture(GL_TEXTURE_2D, 0);

  return texture_id;
}

struct Texture load_texture_from_file(const char* path) {
  struct Texture result = {0};

  Image image = {0};
  if (load_image(path, &image) != 0) {
    fprintf(stderr, "Failed to load image\n");
    return result;
  }
  result.id  = load_texture_from_image(&image);
  result.w = image.width;
  result.h = image.height;

  free(image.pixel_buffer);

  return result;
}

void resources_load() {
  (void)texture_filenames;
  for (i16 i = 0; i < MAX_TEXTURE; i++) {
    char filename[PATH_LENGTH_MAX];
    snprintf(filename, PATH_LENGTH_MAX, "%s/%s.%s", TEXTURE_PATH, texture_filenames[i], TEXTURE_EXT);
    struct Texture texture = load_texture_from_file(filename);
    if (texture.id > 0) {
      textures[i] = texture;
    }
  }
}
