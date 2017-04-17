#ifndef QUADPONG_MENU_SCENE_H
#define QUADPONG_MENU_SCENE_H

#include "quadpong.h"

#include "scene.h"
#include "sprite_text.h"

#define MENU_OPTION_HEIGHT 50

typedef struct menu_option menu_option_t;
typedef struct menu_scene  menu_scene_t;

typedef struct menu_option
{
    menu_option_t * parent;

    sprite_text_t sprite;
    void (*selected)(menu_scene_t *);

    int             num_options;
    menu_option_t * options;

} menu_option_t;

void menu_option_init(menu_option_t * opt,
                      const char *    text,
                      void (*selected)(menu_scene_t *),
                      vec2f_t pos);
menu_option_t * menu_option_add_sub_option(menu_option_t * opt,
                                           const char *    text,
                                           void (*selected)(menu_scene_t *),
                                           vec2f_t pos);

typedef struct menu_scene
{
    scene_t base;

    int             menu_ind;
    sprite_t        title;
    sprite_t        arrow;
    bool            input_wait;
    menu_option_t   root_option_group;
    menu_option_t * cur_option_group;

    struct game_scene * game_scene;
} menu_scene_t;

void menu_scene_init(menu_scene_t * mscn);
void menu_scene_cleanup_cb(scene_t * scn);
void menu_scene_start_cb(scene_t * scn);
void menu_scene_stop_cb(scene_t * scn);
void menu_scene_update_cb(scene_t * scn, SDL_Event * ev, game_time_t * gt);
void menu_scene_render_cb(scene_t * scn);

#endif // QUADPONG_MENU_SCENE_H
