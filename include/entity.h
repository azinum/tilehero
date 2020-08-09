// entity.h

#ifndef _ENTITY_H
#define _ENTITY_H

typedef struct Entity {
  float x, y, w, h;
  float x_speed, y_speed;
  i32 type;
  i32 tile_type;
  i16 sprite_id;
  i16 id;
} Entity;

void entity_init(Entity* e, float x, float y, float w, float h);

void entity_update(Entity* e);

void entity_render(Entity* e);

void entity_render_highlight(Entity* e);

void entity_render_highlight_color(Entity* e, float r, float g, float b);

#endif
