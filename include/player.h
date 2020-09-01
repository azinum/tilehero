// player.h

#ifndef _PLAYER_H
#define _PLAYER_H

struct Player {
  i16 stunned;
};

extern struct Player player;

void player_controller();

void player_update(Entity* e);

#endif
