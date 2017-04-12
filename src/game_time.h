#ifndef QUAPONG_GAME_TIME_H
#define QUAPONG_GAME_TIME_H

#include "quapong.h"

typedef struct game_time
{
    float fps;
    float delta;
    double elapsed;
    double total;
}
game_time_t;

#endif // QUAPONG_GAME_TIME_H
