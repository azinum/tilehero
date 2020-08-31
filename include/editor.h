// editor.h

#ifndef _EDITOR_H
#define _EDITOR_H

#define USE_EDITOR 1

struct Game_state;

void editor_update(struct Game_state* game);

void editor_render(struct Game_state* game);

#endif
