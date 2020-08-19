// player.h

#ifndef _PLAYER_H
#define _PLAYER_H

struct Player {
  i16 stunned;
};

extern struct Player player;

void player_init(Entity* e, i32 x_tile, i32 y_tile, float w, float h);

void player_update(Entity* e);

#endif
