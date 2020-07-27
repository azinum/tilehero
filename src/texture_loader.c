// texture_loader.c

#include <png.h>

#include "common.h"
#include "renderer_common.h"
#include "texture_loader.h"

typedef struct Image {
  u8* pixel_buffer;
  i16 width;
  i16 height;
  i16 depth;
  i16 pitch;
} Image;

static i32 load_png(const char* file_name, struct Image* image);

i32 load_png(const char* file_name, struct Image* image) {
  FILE* file = fopen(file_name, "rb");
  assert(file != NULL);
  if (!file)
    return -1;

  u8 signature[8];
  if (fread(signature, 1, sizeof(signature), file) < 8) {
    fclose(file);
    return -1;
  }
  if (png_sig_cmp(signature, 0, 8)) {
    fclose(file);
    return -1;
  }
  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png) {
    fclose(file);
    return -1;
  }
  png_infop info = png_create_info_struct(png);
  if (!info) {
    fclose(file);
    return -1;
  }
  if (setjmp(png_jmpbuf(png))) {
    png_destroy_read_struct(&png, &info, NULL);
    fclose(file);
    
    if (image->pixel_buffer != NULL) {
      free(image->pixel_buffer);
      image->pixel_buffer = NULL;
    }
    return -1;
  }

  png_init_io(png, file);
  png_set_sig_bytes(png, sizeof(signature));
  png_read_info(png, info);

  image->width = png_get_image_width(png, info);
  image->height = png_get_image_height(png, info);
  image->depth = png_get_bit_depth(png, info);

  if (image->depth < 8)
    png_set_packing(png);

  if (png_get_valid(png, info, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha(png);

  switch (png_get_color_type(png, info)) {
    case PNG_COLOR_TYPE_GRAY: {
      image->pitch = 3 * image->width;
      png_set_gray_to_rgb(png);
      break;
    }

    case PNG_COLOR_TYPE_GRAY_ALPHA: {
      image->pitch = 4 * image->width;
      png_set_gray_to_rgb(png);
      break;
    }

    case PNG_COLOR_TYPE_PALETTE: {
      image->pitch = 3 * image->width;
      png_set_expand(png);
      break;
    }

    case PNG_COLOR_TYPE_RGB: {
      image->pitch = 3 * image->width;
      break;
    }

    case PNG_COLOR_TYPE_RGBA: {
      image->pitch = 4 * image->width;
      break;
    }

    default:
      assert(0);
      return -1;
  }

  u32 bpp = png_get_rowbytes(png, info) / image->width;

  png_set_interlace_handling(png);
  png_read_update_info(png, info);

  image->pixel_buffer = malloc(sizeof(u8) * image->width * image->height * bpp);
  png_bytep rows[image->height];

  u8* pixels = image->pixel_buffer;
  for (i32 i = 0; i < image->height; i++) {
    rows[i] = pixels;
    pixels += image->width * bpp;
  }

  png_read_image(png, rows);
  png_read_end(png, NULL);
  png_destroy_read_struct(&png, &info, NULL);

  fclose(file);
  return 0;
}

u32 load_texture(const char* path) {
  u32 texture_id = 0;
  Image image;
  if (load_png(path, &image) != 0) {
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
