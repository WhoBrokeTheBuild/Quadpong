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

    spr->align = SPRITE_ALIGN_TOP_LEFT;
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

void sprite_calc_align(sprite_t * spr)
{
    assert(NULL != spr);

    if (SPRITE_ALIGN_TOP_LEFT == spr->align)
    {
        spr->dst_rect.x = (int)spr->pos.x;
        spr->dst_rect.y = (int)spr->pos.y;
    }
    else if (SPRITE_ALIGN_CENTER == spr->align)
    {
        spr->dst_rect.x = (int)spr->pos.x - (spr->dst_rect.w / 2);
        spr->dst_rect.y = (int)spr->pos.y - (spr->dst_rect.h / 2);
    }
}

void sprite_set_align(sprite_t * spr, sprite_align_t align)
{
    assert(NULL != spr);

    spr->align = align;
    sprite_calc_align(spr);
}

void sprite_set_pos(sprite_t * spr, vec2f_t pos)
{ 
    assert(NULL != spr);

    spr->pos = pos;
    sprite_calc_align(spr);
}

void sprite_text_init(sprite_text_t * tspr, TTF_Font * font, const char * text)
{
    sprite_t * spr = (sprite_t *)tspr;

    tspr->fast = false;
    tspr->font = font;
    spr->align = SPRITE_ALIGN_TOP_LEFT;
    sprite_text_set_text(tspr, text);

    spr->src_rect.x = spr->src_rect.y = 0;
    spr->dst_rect.x = spr->dst_rect.y = 0;
}

void sprite_text_set_text(sprite_text_t * tspr, const char * text)
{
    sprite_t * spr = (sprite_t *)tspr;

    SDL_Surface * surf;
    SDL_Color color = { 255, 255, 255, 0 };

    if (tspr->fast)
    {
        surf = TTF_RenderText_Solid(tspr->font, text, color);
    }
    else
    {
        surf = TTF_RenderText_Blended(tspr->font, text, color);
    }
    spr->texture = SDL_CreateTextureFromSurface(g_renderer, surf);

    spr->src_rect.w = spr->dst_rect.w = surf->w;
    spr->src_rect.h = spr->dst_rect.h = surf->h;
    sprite_calc_align(spr);
    
    SDL_FreeSurface(surf);
}
