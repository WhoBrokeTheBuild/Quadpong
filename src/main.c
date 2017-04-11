#include "quapong.h"
#include "util.h"
#include "scene.h"

SDL_Window      * g_window = NULL;
SDL_Renderer    * g_renderer = NULL;

TTF_Font * g_fnt_large = NULL;
TTF_Font * g_fnt_small = NULL;

bool g_running = true;

int main(int argc, char ** argv)
{
    int             retval = 0;
    menu_scene_t    menu_scene;

    srand(time(0));

    if (0 > SDL_Init(SDL_INIT_EVERYTHING)) 
    {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        retval = 1;
        goto error_sdl;
    }

    if (SDL_CreateWindowAndRenderer(WIN_WIDTH, WIN_HEIGHT, 0, &g_window, &g_renderer))
    {
        fprintf(stderr, "SDL_CreateWindowAndRenderer failed: %s\n", SDL_GetError());
        retval = 1;
        goto error_sdl;
    }

    SDL_SetWindowTitle(g_window, "Quapong");

    if (0 == (IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        fprintf(stderr, "IMG_Init failed: %s\n", IMG_GetError());
        retval = 1;
        goto error_sdl;
    }

    if (-1 == TTF_Init())
    {
        fprintf(stderr, "TTF_Init failed: %s\n", TTF_GetError());
        retval = 1;
        goto error_sdl;
    }

    g_fnt_large = TTF_OpenFont(FONT_ASSET_PATH, FONT_LARGE_SIZE);
    g_fnt_small = TTF_OpenFont(FONT_ASSET_PATH, FONT_SMALL_SIZE);
    if (NULL == g_fnt_large || NULL == g_fnt_small)
    {
        fprintf(stderr, "TTF_OpenFont failed: %s\n", TTF_GetError());
        retval = 1;
        goto error_assets;
    }

    menu_scene_init(&menu_scene);
    g_cur_scene = (scene_t *)&menu_scene;
 
    SDL_Event ev;

    while (g_running)
    {
        SDL_PollEvent(&ev);
        if (SDL_QUIT == ev.type)
        {
            break;
        }

        SDL_SetRenderDrawColor(g_renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(g_renderer);
        
        if (NULL != g_cur_scene)
        {
            g_cur_scene->update(g_cur_scene, &ev);
        }

        if (NULL != g_cur_scene)
        {
            g_cur_scene->render(g_cur_scene);
        }
    
        SDL_RenderPresent(g_renderer);

        // TODO: Improve
        SDL_Delay(16);
    }

    ((scene_t *)(&menu_scene))->cleanup((scene_t *)&menu_scene);

error_assets:

    TTF_CloseFont(g_fnt_large);
    TTF_CloseFont(g_fnt_small);


error_sdl:

    TTF_Quit();

    IMG_Quit();
    
    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);
    SDL_Quit();

    return retval;
}
