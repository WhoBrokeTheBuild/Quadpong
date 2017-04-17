#include "sprite_text.h"

void _sprite_text_apply(sprite_text_t * tspr);

void
sprite_text_init(sprite_text_t * tspr, TTF_Font * font, const char * text)
{
    sprite_t * spr = (sprite_t *)tspr;
    sprite_init(spr);

    tspr->_fast = false;
    tspr->_font = font;
    tspr->_text = (NULL == text ? NULL : _strdup(text));
    _sprite_text_apply(tspr);
}

void
sprite_text_cleanup(sprite_text_t * tspr)
{
    sprite_t * spr = (sprite_t *)tspr;
    sprite_cleanup(spr);

    free(tspr->_text);
    tspr->_text = NULL;
}

void
sprite_text_set_fast(sprite_text_t * tspr, bool fast)
{
    tspr->_fast = fast;
    _sprite_text_apply(tspr);
}

void
sprite_text_set_text(sprite_text_t * tspr, const char * text)
{
    sprite_t * spr = (sprite_t *)tspr;

    free(tspr->_text);
    if (text == NULL)
    {
        tspr->_text = NULL;
        return;
    }

    tspr->_text = _strdup(text);
    _sprite_text_apply(tspr);
}

void
_sprite_text_apply(sprite_text_t * tspr)
{
    sprite_t *    spr  = (sprite_t *)tspr;
    SDL_Surface * surf = NULL;

    if (NULL != spr->_texture)
    {
        SDL_DestroyTexture(spr->_texture);
        spr->_texture = NULL;
    }

    if (NULL == tspr->_text)
    {
        sprite_set_size(spr, (vec2_t){0, 0});
        return;
    }

    if (tspr->_fast)
    {
        surf = TTF_RenderText_Solid(tspr->_font, tspr->_text, spr->_color);
    }
    else
    {
        surf = TTF_RenderText_Blended(tspr->_font, tspr->_text, spr->_color);
    }
    spr->_texture = SDL_CreateTextureFromSurface(g_renderer, surf);

    spr->_src = (rect_t){0, 0, surf->w, surf->h};
    sprite_set_size(spr, (vec2_t){surf->w, surf->h});

    SDL_FreeSurface(surf);
}

void
sprite_text_set_color(sprite_text_t * tspr, color_t color)
{
    sprite_t * spr = (sprite_t *)tspr;
    sprite_set_color(spr, color);
    _sprite_text_apply(tspr);
}
