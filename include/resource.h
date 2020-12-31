// resource.h

#ifndef _RESOURCE_H
#define _RESOURCE_H

#include "audio.h"

#define SHEET_GET_X_OFFSET(SHEET, INDEX) ((SHEET.w * INDEX) % (SHEET.texture.w))
#define SHEET_GET_Y_OFFSET(SHEET, INDEX) (((SHEET.h * INDEX) / SHEET.texture.w) * SHEET.h)

enum Texture_type {
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
  SPRITE_VOID_WALKER,
  SPRITE_COOKIE,
  SPRITE_FLAG,
  SPRITE_SILVER_KEY,

  MAX_SPRITE,
};

enum Tile_sprite_type {
  TILE_SPRITE_VOID = 0,
  TILE_SPRITE_GROUND,
  TILE_SPRITE_PURPLE_BRICK,
  TILE_SPRITE_BRICK,
  TILE_SPRITE_DUNGEON,
  TILE_SPRITE_SWAPPER,
  TILE_SPRITE_GRASS,

  MAX_TILE_SPRITE,
};

enum Ui_sprite_type {
  UI_SPRITE_CHECK = 0,
  UI_SPRITE_CROSS,
  UI_SPRITE_SURFACE,
};

enum Spritesheet_types {
  SHEET_ENTITIES,
  SHEET_TILES,
  SHEET_UI,

  MAX_SHEET,
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

struct Spritesheet {
  struct Texture texture;
  u32 size;
  u32 w, h;
};

extern struct Texture textures[];
extern struct Audio_source sounds[];
extern struct Sprite_info sprite_info[];
extern struct Spritesheet spritesheets[];

struct Texture load_texture_from_file(const char* path);

void resources_load();

void resource_load_sound(u32 sound_id);

void resources_unload();

#endif
