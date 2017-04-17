#ifndef QUADPONG_SCENE_H
#define QUADPONG_SCENE_H

#include "quadpong.h"

typedef struct scene
{
    void (*start)(scene_t *);
    void (*stop)(scene_t *);

    void (*cleanup)(scene_t *);
    void (*update)(scene_t *, SDL_Event *, game_time_t *);
    void (*render)(scene_t *);

    scene_t * prev;
}
scene_t;

void scene_init(scene_t * scn);
void scene_switch(scene_t * scn);
void scene_push(scene_t * scn);
void scene_pop();

extern scene_t * g_cur_scene;

#endif // QUADPONG_SCENE_H
