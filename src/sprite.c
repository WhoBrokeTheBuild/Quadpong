#include "sprite.h"

bool sprite_load(sprite_t * spr, const char * filename)
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

void sprite_free(sprite_t * spr)
{
    if (NULL == spr) return;

    SDL_DestroyTexture(spr->texture);
}

void sprite_render(sprite_t * spr)
{
    assert(NULL != spr);

    SDL_RenderCopy(g_renderer, spr->texture, &spr->src_rect, &spr->dst_rect);
}
