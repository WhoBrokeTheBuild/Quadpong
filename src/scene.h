#ifndef QUAPONG_SCENE_H
#define QUAPONG_SCENE_H

#include "quapong.h"
#include "vec2.h"

typedef struct scene
{
    void (*start)(struct scene *);
    void (*stop)(struct scene *);

    void (*cleanup)(struct scene *);
    void (*update)(struct scene *, SDL_Event *, float);
    void (*render)(struct scene *);

    struct scene * prev;
}
scene_t;

void scene_init(scene_t * scn);
void scene_switch(scene_t * scn);
void scene_push(scene_t * scn);
void scene_pop();

extern scene_t * g_cur_scene;

#endif // QUAPONG_SCENE_H
