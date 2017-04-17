#ifndef QUADPONG_SPRITE_H
#define QUADPONG_SPRITE_H

#include "quadpong.h"
#include "util.h"
#include "vec2.h"

enum align
{
	ALIGN_TOP_LEFT,
	ALIGN_CENTER,
};
typedef enum align align_t;

typedef struct sprite
{
    SDL_Texture * _texture;
    rect_t		  _src;
    rect_t		  _dst;
	vec2f_t		  _pos;
    color_t		  _color;
    align_t		  _align;
}
sprite_t;

void sprite_init(sprite_t * spr);
bool sprite_load_file(sprite_t * spr, const char * filename);
bool sprite_create(sprite_t * spr, int w, int h, color_t color);
void sprite_cleanup(sprite_t * spr);
void sprite_render(sprite_t * spr);

inline align_t sprite_get_align(sprite_t * spr) { return spr->_align; }
void sprite_set_align(sprite_t * spr, align_t align);

inline vec2f_t sprite_get_pos(sprite_t * spr) { return spr->_pos; }
void sprite_set_pos(sprite_t * spr, vec2f_t pos);

inline vec2_t sprite_get_size(sprite_t * spr) { return (vec2_t) { spr->_dst.w, spr->_dst.h }; }
void sprite_set_size(sprite_t * spr, vec2_t size);

inline color_t sprite_get_color(sprite_t * spr) { return spr->_color; }
void sprite_set_color(sprite_t * spr, color_t color);

inline rect_t sprite_get_bounding_box(sprite_t * spr) { return spr->_dst; }

#endif // QUADPONG_SPRITE_H
