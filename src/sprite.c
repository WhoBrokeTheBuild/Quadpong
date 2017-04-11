#include "sprite.h"

bool sprite_init(sprite_t * spr, const char * filename)
{
    assert(NULL != spr);

    int w, h;
    spr->texture = image_load(filename, g_renderer);

    if (NULL == spr->texture)
    {
        return false;
    }

    SDL_QueryTexture(spr->texture, NULL, NULL, &w, &h);

    spr->src_rect.w = spr->dst_rect.w = w;
    spr->src_rect.h = spr->dst_rect.h = h;
    spr->src_rect.x = spr->src_rect.y = 0;
    spr->dst_rect.x = spr->dst_rect.y = 0;

    return true;
}

void sprite_cleanup(sprite_t * spr)
{
    if (NULL == spr) return;

    SDL_DestroyTexture(spr->texture);
}

void sprite_render(sprite_t * spr)
{
    assert(NULL != spr);

    SDL_RenderCopy(g_renderer, spr->texture, &spr->src_rect, &spr->dst_rect);
}

void sprite_text_init(sprite_text_t * spr, TTF_Font * font, const char * text)
{
    sprite_t * base = (sprite_t *)spr;

    spr->font = font;
    sprite_text_set_text(spr, text);

    base->src_rect.x = base->src_rect.y = 0;
    base->dst_rect.x = base->dst_rect.y = 0;
}

void sprite_text_set_text(sprite_text_t * spr, const char * text)
{
    sprite_t * base = (sprite_t *)spr;

    SDL_Surface * surf;
    SDL_Color color = { 255, 255, 255, 0 };

    surf = TTF_RenderText_Solid(spr->font, text, color);
    base->texture = SDL_CreateTextureFromSurface(g_renderer, surf);

    base->src_rect.w = base->dst_rect.w = surf->w;
    base->src_rect.h = base->dst_rect.h = surf->h;
    
    SDL_FreeSurface(surf);
}
