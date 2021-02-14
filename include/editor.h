// editor.h

#ifndef _EDITOR_H
#define _EDITOR_H

struct Game_state;

extern Tile placable_tiles[];

enum Placable_entity_type {
  ENTITY_PLAYER,
  ENTITY_RED_MONSTER,
  ENTITY_MAD_SCIENTIST,
  ENTITY_WIZARD,
  ENTITY_VOID_WALKER,
  ENTITY_FLAG,
  ENTITY_SILVER_KEY,
  ENTITY_PUSHER,
  ENTITY_SILVER_DOOR,

  MAX_PLACABLE_ENTITY,
};

void editor_update(struct Game_state* game);

struct Entity* editor_place_entity(struct Game_state* game, i16 placable_id, i32 x_tile, i32 y_tile);

struct Entity* editor_place_entity_from_def(struct Entity_def* def);

#endif
