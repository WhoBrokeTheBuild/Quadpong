#include "scene.h"
#include "ball.h"
#include "player.h"

scene_t * g_cur_scene = NULL;

void scene_init(scene_t * scn)
{
    assert(NULL != scn);

    scn->play = NULL;
    scn->stop = NULL;
    scn->cleanup = NULL;
    scn->update = NULL;
    scn->render = NULL;

    scn->prev = NULL;
}

void scene_switch(scene_t * scn)
{
    if (NULL != g_cur_scene)
    {
        g_cur_scene->stop(g_cur_scene);
    }

    g_cur_scene = scn;

    if (NULL != g_cur_scene)
    {
        g_cur_scene->play(g_cur_scene);
    }
}

void menu_option_init(menu_option_t * opt, const char * text, void (*selected)(menu_scene_t *), vec2_t pos)
{
    assert(NULL != opt);

    opt->parent = NULL;

    opt->sprite = NULL;
    if (NULL != text)
    {
        opt->sprite = (sprite_text_t *)malloc(sizeof(sprite_text_t));
        sprite_text_init(opt->sprite, g_fnt_large, text);
        sprite_text_set_pos(opt->sprite, pos);
    }

    opt->num_options = 0;
    opt->selected = selected;
    opt->options = NULL;
}

void menu_option_cleanup(menu_option_t * opt)
{
    sprite_text_cleanup(opt->sprite);
    free(opt->sprite);

    for (int i = 0; i < opt->num_options; ++i)
    {
        menu_option_cleanup(&opt->options[i]);
    }
    free(opt->options);
}

menu_option_t * menu_option_add_sub_option(menu_option_t * opt, const char * text, void (*selected)(menu_scene_t *), vec2_t pos)
{
    assert(NULL != opt);

    ++opt->num_options;
    opt->options = (menu_option_t *)realloc(opt->options, sizeof(menu_option_t) * opt->num_options);
    menu_option_init(&opt->options[opt->num_options - 1], text, selected, pos);
    opt->options[opt->num_options - 1].parent = opt;

    return &opt->options[opt->num_options - 1];
}

void option_quit_cb(menu_scene_t * scn)
{
    g_running = false;
}

void menu_scene_init(menu_scene_t * scn)
{
    assert(NULL != scn);

    scene_t * base = (scene_t *)scn;
    scene_init(base);

    base->play = &menu_scene_play_cb;
    base->stop = &menu_scene_stop_cb;
    base->cleanup = &menu_scene_cleanup_cb;
    base->update = &menu_scene_update_cb;
    base->render = &menu_scene_render_cb;

    scn->menu_ind = 0;
    scn->input_wait = false;

    vec2_t title_pos = { 50, 50 };
    scn->title = (sprite_t *)malloc(sizeof(sprite_t));
    sprite_init(scn->title, TITLE_ASSET_PATH);
    sprite_set_pos(scn->title, title_pos);

    vec2_t arrow_size = { 20, 20 };
    scn->arrow = (sprite_t *)malloc(sizeof(sprite_t));
    sprite_init(scn->arrow, PIXEL_ASSET_PATH);
    sprite_set_size(scn->arrow, arrow_size);

    menu_option_t * menu_ptr;
    vec2_t root_mpos = { 50, 150 };
    menu_option_init(&scn->root_option_group, NULL, NULL, root_mpos);
    scn->cur_option_group = &scn->root_option_group;

    menu_ptr = menu_option_add_sub_option(&scn->root_option_group, "Host Game", NULL, root_mpos);
    root_mpos.y += MENU_OPTION_HEIGHT;

    vec2_t host_mpos = { 50, 150 };
    menu_option_add_sub_option(menu_ptr, "Two Player", NULL, host_mpos);
    host_mpos.y += MENU_OPTION_HEIGHT;
    menu_option_add_sub_option(menu_ptr, "Four Player", NULL, host_mpos);

    menu_ptr = menu_option_add_sub_option(&scn->root_option_group, "Connect to Game", NULL, root_mpos);
    root_mpos.y += MENU_OPTION_HEIGHT;

    menu_ptr = menu_option_add_sub_option(&scn->root_option_group, "Quit", &option_quit_cb, root_mpos);
    root_mpos.y += MENU_OPTION_HEIGHT;
}

void menu_scene_cleanup_cb(scene_t * scn)
{
    menu_scene_t * mscn = (menu_scene_t *)scn;

    menu_option_cleanup(&mscn->root_option_group);

    sprite_cleanup(mscn->title);
}

void menu_scene_play_cb(scene_t * scn)
{

}

void menu_scene_stop_cb(scene_t * scn)
{

}

void menu_scene_update_cb(scene_t * scn, SDL_Event * ev)
{
    menu_scene_t * mscn = (menu_scene_t *)scn;
    
    if (NULL == mscn->cur_option_group)
    {
        return;
    }
    
    vec2_t arrow_pos = { 20, 170 };
    arrow_pos.y += (mscn->menu_ind * MENU_OPTION_HEIGHT);
    sprite_set_pos(mscn->arrow, arrow_pos);

    if (SDL_KEYDOWN == ev->type)
    {
        if (SDLK_UP == ev->key.keysym.sym)
        {
            if (!mscn->input_wait)
            {
                mscn->input_wait = true;
                --mscn->menu_ind;
                if (0 > mscn->menu_ind)
                {
                    mscn->menu_ind = mscn->cur_option_group->num_options - 1;
                }
            }
        }
        else if (SDLK_DOWN == ev->key.keysym.sym)
        {
            if (!mscn->input_wait)
            {
                mscn->input_wait = true;
                ++mscn->menu_ind;
                if (mscn->menu_ind >= mscn->cur_option_group->num_options)
                {
                    mscn->menu_ind = 0;
                }
            }
        }
        else if (SDLK_RETURN == ev->key.keysym.sym)
        {
            if (!mscn->input_wait)
            {
                mscn->input_wait = true;
                menu_option_t * opt = &mscn->cur_option_group->options[mscn->menu_ind];
                if (NULL != opt->selected)
                {
                    opt->selected(mscn);
                }
                else
                {
                    mscn->menu_ind = 0;
                    mscn->cur_option_group = opt;
                }
            }
        }
        else if (SDLK_ESCAPE == ev->key.keysym.sym)
        {
            if (!mscn->input_wait)
            {
                mscn->input_wait = true;
                if (NULL != mscn->cur_option_group->parent)
                {
                    mscn->menu_ind = 0;
                    mscn->cur_option_group = mscn->cur_option_group->parent;
                }
            }
        }
    }
    else if (SDL_KEYUP == ev->type)
    {
        mscn->input_wait = false;
    }
}

void menu_scene_render_cb(scene_t * scn)
{
    menu_scene_t * mscn = (menu_scene_t *)scn;

    sprite_render(mscn->title);
    sprite_render(mscn->arrow);

    if (mscn->cur_option_group)
    {
        for (int i = 0; i < mscn->cur_option_group->num_options; ++i)
        {
            sprite_text_render(mscn->cur_option_group->options[i].sprite);
        }
    }
}

void game_scene_init(game_scene_t * scn)
{
    assert(NULL != scn);

    scene_t * base = (scene_t *)scn;
    scene_init(base);

    base->play = &game_scene_play_cb;
    base->stop = &game_scene_stop_cb;
    base->cleanup = &game_scene_cleanup_cb;
    base->update = &game_scene_update_cb;
    base->render = &game_scene_render_cb;

    ball_init(scn->ball);

    memset(scn->players, 0, sizeof(player_t *) * MAX_PLAYERS);

    scn->players[0] = (player_t *)malloc(sizeof(local_player_t));
    local_player_init((local_player_t *)scn->players[0],
            AREA_LEFT, SDLK_DOWN, SDLK_UP);

    scn->players[1] = (player_t *)malloc(sizeof(local_player_t));
    local_player_init((local_player_t *)scn->players[1],
            AREA_RIGHT, SDLK_s, SDLK_w);

    scn->players[2] = (player_t *)malloc(sizeof(local_player_t));
    local_player_init((local_player_t *)scn->players[2],
            AREA_TOP, SDLK_RIGHT, SDLK_LEFT);

    scn->players[3] = (player_t *)malloc(sizeof(local_player_t));
    local_player_init((local_player_t *)scn->players[3],
            AREA_BOTTOM, SDLK_d, SDLK_a);

}

void game_scene_cleanup_cb(scene_t * scn)
{
    game_scene_t * gscn = (game_scene_t *)scn;

    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
        if (gscn->players[i])
        {
            if (gscn->players[i]->cleanup)
            {
                gscn->players[i]->cleanup(gscn->players[i]);
            }
            free(gscn->players[i]);
        }
    }

    ball_cleanup(gscn->ball);
    free(gscn->ball);
}

void game_scene_play_cb(scene_t * scn)
{
    game_scene_t * gscn = (game_scene_t *)scn;

}

void game_scene_stop_cb(scene_t * scn)
{
    game_scene_t * gscn = (game_scene_t *)scn;

}

void game_scene_update_cb(scene_t * scn, SDL_Event * ev)
{
    game_scene_t * gscn = (game_scene_t *)scn;

    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
        if (NULL != gscn->players[i])
        {
            if (gscn->players[i]->update)
            {
                gscn->players[i]->update(gscn->players[i], ev);
            }
        }
    }

    ball_update(gscn->ball, gscn->players);
}

void game_scene_render_cb(scene_t * scn)
{
    game_scene_t * gscn = (game_scene_t *)scn;

    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
        if (NULL != gscn->players[i])
        {
            if (gscn->players[i]->render)
            {
                gscn->players[i]->render(gscn->players[i]);
            }
        }
    }

    ball_render(gscn->ball);
}
