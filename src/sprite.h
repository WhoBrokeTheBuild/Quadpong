#ifndef QUADPONG_SPRITE_H
#define QUADPONG_SPRITE_H

#include "quadpong.h"
#include "util.h"
#include "vec2.h"

typedef enum sprite_align
{
    SPRITE_ALIGN_TOP_LEFT,
    SPRITE_ALIGN_CENTER,
}
sprite_align_t;

typedef struct sprite
{
    SDL_Texture * texture;
    SDL_Rect src_rect;
    SDL_Rect dst_rect;
    SDL_Color color;
    vec2f_t pos;
    sprite_align_t align;
}
sprite_t;

bool sprite_init(sprite_t * spr, const char * filename);
void sprite_cleanup(sprite_t * spr);
void sprite_render(sprite_t * spr);

void sprite_calc_align(sprite_t * spr);

void sprite_set_align(sprite_t * spr, sprite_align_t align);
void sprite_set_pos(sprite_t * spr, vec2f_t pos);

static inline sprite_align_t sprite_get_align(sprite_t * spr)
{
    assert(NULL != spr);

    return spr->align;
}

static inline vec2f_t sprite_get_pos(sprite_t * spr) 
{ 
    assert(NULL != spr);

    return spr->pos;
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

typedef struct sprite_text
{
    sprite_t base;

    bool fast;
    TTF_Font * font;
}
sprite_text_t;

void sprite_text_init(sprite_text_t * spr, TTF_Font * font, const char * text);
void sprite_text_set_text(sprite_text_t * spr, const char * text);

static inline void sprite_text_cleanup(sprite_text_t * spr)
{
    sprite_cleanup((sprite_t *)spr);
}

static inline void sprite_text_render(sprite_text_t * spr)
{
    sprite_render((sprite_t *)spr);
}

static inline sprite_align_t sprite_text_get_align(sprite_text_t * spr) 
{ 
    return sprite_get_align((sprite_t *)spr);
}

static inline void sprite_text_set_align(sprite_text_t * spr, sprite_align_t align) 
{ 
    sprite_set_align((sprite_t *)spr, align);
}

static inline vec2f_t sprite_text_get_pos(sprite_text_t * spr) 
{ 
    return sprite_get_pos((sprite_t *)spr);
}

static inline void sprite_text_set_pos(sprite_text_t * spr, vec2f_t pos) 
{ 
    sprite_set_pos((sprite_t *)spr, pos);
}

#endif // QUADPONG_SPRITE_H
