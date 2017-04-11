#ifndef QUAPONG_SCENE_H
#define QUAPONG_SCENE_H

#include "quapong.h"
#include "vec2.h"

typedef struct scene
{
    void (*play)(struct scene *);
    void (*stop)(struct scene *);

    void (*cleanup)(struct scene *);
    void (*update)(struct scene *, SDL_Event *);
    void (*render)(struct scene *);

    struct scene * prev;
}
scene_t;

void scene_init(scene_t * scn);
void scene_switch(scene_t * scn);

struct sprite;
struct sprite_text;
struct menu_scene;

#define MENU_OPTION_HEIGHT 50

typedef struct menu_option
{
    struct menu_option * parent;
    
    struct sprite_text * sprite;
    void (*selected)(struct menu_scene *);

    int num_options;
    struct menu_option * options;
}
menu_option_t;

void menu_option_init(menu_option_t * opt, const char * text, void (*selected)(struct menu_scene *), vec2_t pos);
menu_option_t * menu_option_add_sub_option(menu_option_t * opt, const char * text, void (*selected)(struct menu_scene *), vec2_t pos);

typedef struct menu_scene
{
    scene_t base;

    int menu_ind;
    struct sprite * title;
    struct sprite * arrow;
    bool input_wait;
    menu_option_t root_option_group;
    menu_option_t * cur_option_group;
}
menu_scene_t;

void menu_scene_init(menu_scene_t * scn);
void menu_scene_cleanup_cb(scene_t * scn);
void menu_scene_play_cb(scene_t * scn);
void menu_scene_stop_cb(scene_t * scn);
void menu_scene_update_cb(scene_t * scn, SDL_Event * ev);
void menu_scene_render_cb(scene_t * scn);

struct player;
struct ball;

typedef struct game_scene
{
    scene_t base;

    struct player * players[MAX_PLAYERS];
    struct ball *   ball;
}
game_scene_t;

void game_scene_init(game_scene_t * scn);
void game_scene_cleanup_cb(scene_t * scn);
void game_scene_play_cb(scene_t * scn);
void game_scene_stop_cb(scene_t * scn);
void game_scene_update_cb(scene_t * scn, SDL_Event * ev);
void game_scene_render_cb(scene_t * scn);

extern scene_t * g_cur_scene;

#endif // QUAPONG_SCENE_H
