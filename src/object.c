#include "object.h"
#include "game_time.h"

void object_init(object_t *obj)
{
    sprite_init(&obj->_sprite);
    obj->_pos = (vec2f_t){0.0f, 0.0f};
    obj->_vel = (vec2f_t){0.0f, 0.0f};
}

void object_cleanup(object_t *obj)
{
    sprite_cleanup(&obj->_sprite);
}

void object_update(object_t *obj, game_time_t *gt)
{
    obj->_pos = vec2f_add(obj->_pos, vec2f_muls(obj->_vel, gt->delta));

    sprite_set_pos(&obj->_sprite, obj->_pos);
}

void object_render(object_t *obj)
{
    sprite_render(&obj->_sprite);
}

void object_set_pos(object_t *obj, vec2f_t pos)
{
    obj->_pos = pos;
    sprite_set_pos(&obj->_sprite, pos);
}

void object_set_vel(object_t *obj, vec2f_t vel)
{
    obj->_vel = vel;
}
