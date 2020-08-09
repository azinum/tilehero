// resource.c

#include <png.h>

#include "common.h"
#include "image_loader.h"
#include "renderer_common.h"
#include "riff.h"
#include "audio.h"
#include "resource.h"

#define TEXTURE_PATH "resource/texture"
#define SOUND_PATH "resource/audio"
#define TEXTURE_EXT "png"
#define SOUND_EXT "wav"

struct Texture textures[MAX_TEXTURE];
struct Audio_source sounds[MAX_SOUND];

static const char* texture_filenames[] = {
  "spritesheet",
  "font",
};

static const char* sound_filenames[] = {
  "hit_hurt",
  "random_1",
  "random_2",
  "good_morning",

  "metaking",
};

static u32 load_texture_from_image(struct Image* image);

u32 load_texture_from_image(struct Image* image) {
  u32 texture_id = 0;

  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

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

// NOTE(lucas): All resources are loaded even though we're not using all of them.
// Maybe we should load resources when they are needed; when they are requested?
// But how (or when rather) do we unload the resources?
void resources_load() {
  for (i16 i = 0; i < MAX_TEXTURE; i++) {
    char filename[MAX_PATH_LENGTH] = {0};
    snprintf(filename, MAX_PATH_LENGTH, "%s/%s.%s", TEXTURE_PATH, texture_filenames[i], TEXTURE_EXT);
    struct Texture texture = load_texture_from_file(filename);
    if (texture.id > 0) {
      textures[i] = texture;
    }
  }
}

// NOTE(lucas): We might want to figure out before-hand which resources we need.
// What we do now is that we load the resources at the time we need them (audio resources). This might be
// a bad thing because of the 'blocking' nature of loading resources. At the moment we are loading resources
// sequentially with the game, so we might get laggy frames because we are waiting for resources to be loaded!
void resource_load_sound(i32 sound_id) {
  if (sound_id < 0 || sound_id >= MAX_SOUND) {
    fprintf(stderr, "Failed to load sound (no such sound id: %i)\n", sound_id);
    return;
  }
  char filename[MAX_PATH_LENGTH] = {0};
  snprintf(filename, MAX_PATH_LENGTH, "%s/%s.%s", SOUND_PATH, sound_filenames[sound_id], SOUND_EXT);
  struct Audio_source source = {0};
  load_wav_from_file(filename, &source);
  sounds[sound_id] = source;
}

void resources_unload() {
  for (i32 i = 0; i < MAX_SOUND; i++) {
    struct Audio_source* source = &sounds[i];
    if (source->sample_buffer) {
      free(source->sample_buffer);
      source->sample_count = 0;
      source->sample_rate = 0;
    }
  }
}
