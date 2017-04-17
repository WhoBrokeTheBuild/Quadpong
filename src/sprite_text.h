#ifndef QUADPONG_SPRITE_TEXT_H
#define QUADPONG_SPRITE_TEXT_H

#include "quadpong.h"
#include "sprite.h"

typedef struct sprite_text
{
    sprite_t base;

    bool       _fast;
    TTF_Font * _font;
    char *     _text;

} sprite_text_t;

void sprite_text_init(sprite_text_t * tspr, TTF_Font * font, const char * text);
void sprite_text_cleanup(sprite_text_t * tspr);
static inline void
sprite_text_render(sprite_text_t * tspr)
{
    sprite_render((sprite_t *)tspr);
}

static inline align_t
sprite_text_get_align(sprite_text_t * tspr)
{
    return sprite_get_align((sprite_t *)tspr);
}
static inline void
sprite_text_set_align(sprite_text_t * tspr, align_t align)
{
    sprite_set_align((sprite_t *)tspr, align);
}

static inline vec2f_t
sprite_text_get_pos(sprite_text_t * tspr)
{
    return sprite_get_pos((sprite_t *)tspr);
}
static inline void
sprite_text_set_pos(sprite_text_t * tspr, vec2f_t pos)
{
    sprite_set_pos((sprite_t *)tspr, pos);
}

static inline color_t
sprite_text_get_color(sprite_text_t * tspr)
{
    return sprite_get_color((sprite_t *)tspr);
}
void sprite_text_set_color(sprite_text_t * tspr, color_t color);

static inline bool
sprite_text_is_fast(sprite_text_t * tspr)
{
    return tspr->_fast;
}
void sprite_text_set_fast(sprite_text_t * tspr, bool fast);

void sprite_text_set_text(sprite_text_t * tspr, const char * text);

#endif // QUADPONG_SPRITE_TEXT_H
