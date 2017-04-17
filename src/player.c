#include "player.h"

void player_init(player_t *ply, area_t area, SDL_Color color)
{
    object_t *obj = &ply->base;
    object_init(obj);

    ply->cleanup = &player_cleanup_cb;
    ply->update = NULL;
    ply->render = &player_render_cb;
    ply->_score = 0;
    ply->_area = area;

    sprite_t *spr = object_get_sprite(obj);
    vec2f_t pos;

    sprite_create(spr, 1, 1, (color_t){255, 255, 255, 255});
    sprite_set_align(spr, ALIGN_CENTER);

    if (AREA_LEFT == ply->_area)
    {
        pos = (vec2f_t){WALL_PADDING + (PLAYER_V_WIDTH / 2), WIN_HEIGHT / 2};
        sprite_set_size(spr, (vec2_t){PLAYER_V_WIDTH, PLAYER_V_HEIGHT});
    }
    if (AREA_RIGHT == ply->_area)
    {
        pos = (vec2f_t){WIN_WIDTH - (WALL_PADDING + (PLAYER_V_WIDTH / 2)), WIN_HEIGHT / 2};
        sprite_set_size(spr, (vec2_t){PLAYER_V_WIDTH, PLAYER_V_HEIGHT});
    }
    else if (AREA_TOP == ply->_area)
    {
        pos = (vec2f_t){WIN_WIDTH / 2, WALL_PADDING + (PLAYER_H_HEIGHT / 2)};
        sprite_set_size(spr, (vec2_t){PLAYER_H_WIDTH, PLAYER_H_HEIGHT});
    }
    else if (AREA_BOTTOM == ply->_area)
    {
        pos = (vec2f_t){WIN_WIDTH / 2, WIN_HEIGHT - (WALL_PADDING + (PLAYER_H_HEIGHT / 2))};
        sprite_set_size(spr, (vec2_t){PLAYER_H_WIDTH, PLAYER_H_HEIGHT});
    }

    object_set_pos(obj, pos);
    object_set_color(obj, color);
}

void player_cleanup_cb(player_t *ply)
{
    object_cleanup(&ply->base);
}

void player_render_cb(player_t *ply)
{
    assert(NULL != ply);

    object_render(&ply->base);
}

void local_player_init(local_player_t *ply, area_t area, SDL_Color color, SDL_Keycode forward,
                       SDL_Keycode back)
{
    assert(NULL != ply);

    player_t *base = (player_t *)ply;
    player_init(base, area, color);

    base->update = &local_player_update_cb;

    ply->dir = 0;
    ply->key_forward = forward;
    ply->key_back = back;
}

void local_player_update_cb(player_t *ply, SDL_Event *ev, game_time_t *gt)
{
    local_player_t *lply = (local_player_t *)ply;

    vec2f_t vel = player_get_vel(ply);

    vec2f_t mod = {0.0f, 0.0f};
    if (AREA_LEFT == ply->_area || AREA_RIGHT == ply->_area)
    {
        mod.y = 1.0f;
    }
    else
    {
        mod.x = 1.0f;
    }

    if (SDL_KEYDOWN == ev->type)
    {
        if (ev->key.keysym.sym == lply->key_forward)
        {
            vel.x = PLAYER_SPEED * mod.x;
            vel.y = PLAYER_SPEED * mod.y;
        }
        else if (ev->key.keysym.sym == lply->key_back)
        {
            vel.x = -PLAYER_SPEED * mod.x;
            vel.y = -PLAYER_SPEED * mod.y;
        }
    }
    else if (SDL_KEYUP == ev->type)
    {
        if (ev->key.keysym.sym == lply->key_forward)
        {
            if (vel.x * mod.x > 0.0f || vel.y * mod.y > 0.0f)
            {
                vel.x = vel.y = 0.0f;
            }
        }
        else if (ev->key.keysym.sym == lply->key_back)
        {
            if (vel.x * mod.x < 0.0f || vel.y * mod.y < 0.0f)
            {
                vel.x = vel.y = 0.0f;
            }
        }
    }

    // TODO: Movement

    player_set_vel(ply, vel);
    object_update(&ply->base, gt);
}

void network_player_init(network_player_t *nply, area_t area, SDL_Color color,
                         struct sockaddr_in *addr, socklen_t addrlen)
{
    player_t *ply = (player_t *)nply;
    player_init(ply, area, color);

    memcpy(&nply->addr, addr, addrlen);
    nply->ttl = MAX_PLAYER_TTL;
}

void network_player_cleanup_cb(player_t *nply)
{
}

void network_player_update_cb(player_t *nply, SDL_Event *ev, game_time_t *gt)
{
}
