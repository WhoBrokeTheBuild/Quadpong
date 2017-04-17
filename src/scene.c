#include "scene.h"

scene_t *g_cur_scene = NULL;

void scene_init(scene_t *scn)
{
    scn->start = NULL;
    scn->stop = NULL;
    scn->cleanup = NULL;
    scn->update = NULL;
    scn->render = NULL;

    scn->prev = NULL;
}

void scene_switch(scene_t *scn)
{
    if (NULL != g_cur_scene)
    {
        g_cur_scene->stop(g_cur_scene);
    }

    g_cur_scene = scn;

    if (NULL != g_cur_scene)
    {
        g_cur_scene->start(g_cur_scene);
    }
}

void scene_push(scene_t *scn)
{
    scn->prev = g_cur_scene;
    scene_switch(scn);
}

void scene_pop()
{
    scene_t *scn = g_cur_scene->prev;
    g_cur_scene->prev = NULL;
    scene_switch(scn);
}
