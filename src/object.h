#ifndef QUADPONG_OBJECT_H
#define QUADPONG_OBJECT_H

#include "quadpong.h"
#include "sprite.h"
#include "vec2.h"

typedef struct object
{
	sprite_t _sprite;
	vec2f_t	 _pos;
	vec2f_t	 _vel;
}
object_t;

void object_init(object_t * obj);
void object_cleanup(object_t * obj);
void object_update(object_t * obj, game_time_t * gt);
void object_render(object_t * obj);

static inline sprite_t * object_get_sprite(object_t * obj) { return &obj->_sprite; }

static inline color_t object_get_color(object_t * obj) { return sprite_get_color(&obj->_sprite); }
static inline void object_set_color(object_t * obj, color_t color) { sprite_set_color(&obj->_sprite, color); }

static inline vec2f_t object_get_vel(object_t * obj) { return obj->_vel; }
void object_set_vel(object_t * obj, vec2f_t vel);

static inline vec2f_t object_get_pos(object_t * obj) { return obj->_pos; }
void object_set_pos(object_t * obj, vec2f_t pos);

static inline rect_t object_get_bounding_box(object_t * obj) { return sprite_get_bounding_box(&obj->_sprite); }

#endif // !QUADPONG_OBJECT_H
