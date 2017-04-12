#ifndef QUAPONG_BALL_H
#define QUAPONG_BALL_H

#include "quapong.h"
#include "game_time.h"
#include "sprite.h"
#include "vec2.h"

typedef struct ball
{
    sprite_t * sprite;
    int last_hit;
    SDL_Color color;
    vec2f_t vel;
}
ball_t;

struct player;

void ball_init(ball_t * ball);
void ball_cleanup(ball_t * ball);

void ball_update(ball_t * ball, struct player ** players, game_time_t * gt);
void ball_render(ball_t * ball);

#endif // QUAPONG_BALL_H
