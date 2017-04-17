#ifndef QUADPONG_PLAYER_H
#define QUADPONG_PLAYER_H

#include "quadpong.h"
#include "object.h"

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
	object_t base;

    area_t _area;
    int	   _score;

	void(*cleanup)(player_t *);
	void(*update)(player_t *, SDL_Event *, game_time_t *);
	void(*render)(player_t *);
}
player_t;

void player_init(player_t * ply, area_t area, SDL_Color color);
void player_cleanup_cb(player_t * ply);
void player_render_cb(player_t * ply);

static inline color_t player_get_color(player_t * player) { return object_get_color(&player->base); }
static inline void player_set_color(player_t * player, color_t color) { object_set_color(&player->base, color); }

static inline vec2f_t player_get_vel(player_t * player) { return object_get_vel(&player->base); }
static inline void player_set_vel(player_t * player, vec2f_t vel) { object_set_vel(&player->base, vel); }

static inline vec2f_t player_get_pos(player_t * ply) { return object_get_pos(&ply->base); }
static inline void player_set_pos(player_t * ply, vec2f_t pos) { object_set_pos(&ply->base, pos); }

static inline area_t player_get_area(player_t * ply) { return ply->_area; }
static inline SDL_Rect player_get_bounding_box(player_t * ply) { return object_get_bounding_box(&ply->base); }

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
