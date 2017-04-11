#ifndef QUAPONG_BALL_H
#define QUAPONG_BALL_H

#include "quapong.h"
#include "sprite.h"
#include "vec2.h"

typedef struct ball
{
    sprite_t * sprite;
    SDL_Color color;
    vec2_t vel;
}
ball_t;

struct player;

void ball_init(ball_t * ball);

void ball_update(ball_t * ball, struct player ** players);
void ball_render(ball_t * ball);

#endif // QUAPONG_BALL_H
