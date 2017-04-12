#ifndef QUADPONG_SCENE_H
#define QUADPONG_SCENE_H

#include "quadpong.h"
#include "game_time.h"
#include "vec2.h"

typedef struct scene
{
    void (*start)(struct scene *);
    void (*stop)(struct scene *);

    void (*cleanup)(struct scene *);
    void (*update)(struct scene *, SDL_Event *, game_time_t *);
    void (*render)(struct scene *);

    struct scene * prev;
}
scene_t;

void scene_init(scene_t * scn);
void scene_switch(scene_t * scn);
void scene_push(scene_t * scn);
void scene_pop();

extern scene_t * g_cur_scene;

#endif // QUADPONG_SCENE_H
