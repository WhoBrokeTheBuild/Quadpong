#ifndef QUAPONG_PLAYER_H
#define QUAPONG_PLAYER_H

#include "quapong.h"
#include "sprite.h"
#include "vec2.h"

typedef enum player_area
{
    AREA_TOP,
    AREA_BOTTOM,
    AREA_LEFT,
    AREA_RIGHT
}
area_t;

typedef struct player 
{
    void (*cleanup)(struct player *);
    void (*update)(struct player *, SDL_Event *, float);
    void (*render)(struct player *);

    sprite_t * sprite;
    SDL_Color color;
    int movement;
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
void local_player_update_cb(player_t * ply, SDL_Event * ev, float delta);

typedef struct network_player
{
    player_t base;

    int id;
}
network_player_t;

void network_player_init(network_player_t * ply, area_t area, SDL_Color color, int id);
void network_player_cleanup_cb(player_t * ply);
void network_player_update_cb(player_t * ply, SDL_Event * ev, float delta);

#endif // QUAPONG_PLAYER_H
