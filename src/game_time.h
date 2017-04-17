#ifndef QUADPONG_GAME_TIME_H
#define QUADPONG_GAME_TIME_H

#include "quadpong.h"

typedef struct game_time
{
    float fps;
    float delta;
    double elapsed;
    double total;
} game_time_t;

#endif // QUADPONG_GAME_TIME_H
