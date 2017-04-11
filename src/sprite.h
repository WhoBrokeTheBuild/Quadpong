#ifndef QUAPONG_SPRITE_H
#define QUAPONG_SPRITE_H

#include "quapong.h"
#include "util.h"
#include "vec2.h"

typedef struct sprite
{
    SDL_Texture * texture;
    SDL_Rect src_rect;
    SDL_Rect dst_rect;
    SDL_Color color;
}
sprite_t;

bool sprite_load(sprite_t * spr, const char * filename);
void sprite_free(sprite_t * spr);

void sprite_render(sprite_t * spr);

static inline vec2_t sprite_get_pos(sprite_t * spr) 
{ 
    assert(NULL != spr);

    vec2_t ret = { spr->dst_rect.x, spr->dst_rect.y };
    return ret;
}

static inline void sprite_set_pos(sprite_t * spr, vec2_t pos) 
{ 
    assert(NULL != spr);

    spr->dst_rect.x = pos.x; 
    spr->dst_rect.y = pos.y; 
}

static inline vec2_t sprite_get_size(sprite_t * spr) 
{ 
    assert(NULL != spr);

    vec2_t ret = { spr->dst_rect.w, spr->dst_rect.h };
    return ret;
}

static inline void sprite_set_size(sprite_t * spr, vec2_t size) 
{ 
    assert(NULL != spr);

    spr->dst_rect.w = size.x; 
    spr->dst_rect.h = size.y; 
}

#endif // QUAPONG_SPRITE_H
