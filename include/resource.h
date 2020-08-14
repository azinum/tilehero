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

enum Sprite_type {
  SPRITE_BOY_WITH_HELM = 0,
  SPRITE_BOY,
  SPRITE_WIZARD,
  SPRITE_RED_MONSTER,
  SPRITE_MAD_SCIENTIST,

  MAX_SPRITE,
};

enum Tile_sprite_type {
  TILE_SPRITE_VOID = MAX_SPRITE,
  TILE_SPRITE_GROUND,
  TILE_SPRITE_PURPLE_BRICK,
  TILE_SPRITE_BRICK,
  TILE_SPRITE_DUNGEON,
  TILE_SPRITE_FLIPPER,

  MAX_TILE_SPRITE,
};

struct Texture {
  u32 id;
  i16 w;
  i16 h;
};

struct Sprite_info {
  i16 x_offset;
  i16 y_offset;
  i16 x_range;
  i16 y_range;
};

extern struct Texture textures[];
extern struct Audio_source sounds[];
extern struct Sprite_info sprite_info[];

struct Texture load_texture_from_file(const char* path);

void resources_load();

void resource_load_sound(i32 sound_id);

void resources_unload();

#endif
