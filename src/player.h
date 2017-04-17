#ifndef QUADPONG_PLAYER_H
#define QUADPONG_PLAYER_H

#include "quadpong.h"
#include "game_time.h"
#include "sprite.h"
#include "vec2.h"

typedef enum
{
    AREA_TOP     = 1,
    AREA_BOTTOM  = 2,
    AREA_LEFT    = 3,
    AREA_RIGHT   = 4,
}
area_t;

typedef struct player
{
    void (*cleanup)(struct player *);
    void (*update)(struct player *, SDL_Event *, game_time_t *);
    void (*render)(struct player *);

    sprite_t * sprite;
    SDL_Color color;
    vec2f_t pos;
    vec2f_t vel;
    area_t area;
    int score;
}
player_t;

void player_init(player_t * ply, area_t area, SDL_Color color);
void player_cleanup_cb(player_t * ply);
void player_render_cb(player_t * ply);

typedef struct local_player
{
    player_t base;

    int dir;
    SDL_Keycode key_forward;
    SDL_Keycode key_back;
}
local_player_t;

void local_player_init(local_player_t * ply, area_t area, SDL_Color color, SDL_Keycode forward, SDL_Keycode back);
void local_player_update_cb(player_t * ply, SDL_Event * ev, game_time_t * gt);

typedef struct network_player
{
    player_t base;

    struct sockaddr_in addr;
    double ttl;
}
network_player_t;

void network_player_init(network_player_t * ply, area_t area, SDL_Color color, struct sockaddr_in * addr, socklen_t addrlen);
void network_player_cleanup_cb(player_t * ply);
void network_player_update_cb(player_t * ply, SDL_Event * ev, game_time_t * gt);

#endif // QUADPONG_PLAYER_H
