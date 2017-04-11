#include "player.h"

SDL_Color player_colors[] = {
    { 255,   0,   0, 255 },
    {   0, 255,   0, 255 },
    {   0,   0, 255, 255 },
    { 255,   0, 255, 255 }
};

void player_init(player_t * ply, area_t area)
{
    assert(NULL != ply);

    static int player_ind = -1;
    ++player_ind;

    ply->cleanup = &player_cleanup_cb;
    ply->update = NULL;
    ply->render = &player_render_cb;
    ply->area = area;
    ply->movement = 50;

    ply->sprite = (sprite_t *)malloc(sizeof(sprite_t));
    sprite_load(ply->sprite, PIXEL_ASSET_PATH);

    if (AREA_LEFT == ply->area || AREA_RIGHT == ply->area)
    {
        vec2_t size = { PLAYER_V_WIDTH, PLAYER_V_HEIGHT };
        sprite_set_size(ply->sprite, size);
    }
    else if (AREA_TOP == ply->area || AREA_BOTTOM == ply->area)
    {
        vec2_t size = { PLAYER_H_WIDTH, PLAYER_H_HEIGHT };
        sprite_set_size(ply->sprite, size);
    }

    SDL_SetTextureColorMod(ply->sprite->texture, 
            player_colors[player_ind].r,
            player_colors[player_ind].g,
            player_colors[player_ind].b);
}

void player_cleanup_cb(player_t * ply)
{
    assert(NULL != ply);

    sprite_free(ply->sprite);
    free(ply->sprite);
}

void player_render_cb(player_t * ply)
{
    assert(NULL != ply);

    vec2_t pos = { 0, 0 };

    switch (ply->area)
    {
    case AREA_LEFT:

        pos.x = (PLAYER_V_WIDTH / 2);
        pos.y = ((ply->movement / 100.0f) * (WIN_HEIGHT - PLAYER_V_HEIGHT - (CORNER_SIZE * 2))) + CORNER_SIZE;

        break;
    case AREA_RIGHT:

        pos.x = WIN_WIDTH - (PLAYER_V_WIDTH / 2) - PLAYER_V_WIDTH;
        pos.y = ((ply->movement / 100.0f) * (WIN_HEIGHT - PLAYER_V_HEIGHT - (CORNER_SIZE * 2))) + CORNER_SIZE;

        break;
    case AREA_TOP:

        pos.x = ((ply->movement / 100.0f) * (WIN_WIDTH - PLAYER_H_WIDTH - (CORNER_SIZE * 2))) + CORNER_SIZE;
        pos.y = (PLAYER_H_HEIGHT / 2);

        break;
    case AREA_BOTTOM:

        pos.x = ((ply->movement / 100.0f) * (WIN_WIDTH - PLAYER_H_WIDTH - (CORNER_SIZE * 2))) + CORNER_SIZE;
        pos.y = WIN_HEIGHT - (PLAYER_H_HEIGHT / 2) - PLAYER_H_HEIGHT;

        break;
    }

    sprite_set_pos(ply->sprite, pos);
    sprite_render(ply->sprite);
}

void local_player_init(local_player_t * ply, area_t area, SDL_Keycode forward, SDL_Keycode back)
{
    assert(NULL != ply);

    player_t * base = (player_t *)ply;
    player_init(base, area);

    base->update = &local_player_update_cb;

    ply->dir = 0;
    ply->key_forward = forward;
    ply->key_back = back;
}

void local_player_update_cb(player_t * ply, SDL_Event * ev)
{
    assert(NULL != ply);

    local_player_t * lply = (local_player_t *)ply;
    if (SDL_KEYDOWN == ev->type)
    {
        if (ev->key.keysym.sym == lply->key_forward)
        {
            lply->dir = 1;
        }
        else if (ev->key.keysym.sym == lply->key_back)
        {
            lply->dir = -1;    
        }
    }
    else if (SDL_KEYUP == ev->type)
    {
        if (ev->key.keysym.sym == lply->key_forward)
        {
            if (0 < lply->dir)
            {
                lply->dir = 0;
            }
        }
        else if (ev->key.keysym.sym == lply->key_back)
        {
            if (0 > lply->dir)
            {
                lply->dir = 0;
            }
        }
    }
    
    if (0 < lply->dir)
    {
        ++ply->movement;
        if (100 < ply->movement)
        {
            ply->movement = 100;
        }
    }
    else if (0 > lply->dir)
    {
        --ply->movement;
        if (0 > ply->movement)
        {
            ply->movement = 0;
        }
    }
}

void network_player_init(network_player_t * ply, area_t area, int sock)
{

}

void network_player_cleanup_cb(player_t * ply)
{

}

void network_player_update_cb(player_t * ply, SDL_Event * ev)
{

}
