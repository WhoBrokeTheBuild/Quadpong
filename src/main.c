#include "quapong.h"
#include "util.h"
#include "menu_scene.h"
#include "game_time.h"

SDL_Window      * g_window = NULL;
SDL_Renderer    * g_renderer = NULL;

TTF_Font * g_fnt_large = NULL;
TTF_Font * g_fnt_small = NULL;

bool g_running  = true;
bool g_cap_fps  = true;
bool g_show_fps = false;
int g_max_fps   = 60;

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
    scene_switch((scene_t *)&menu_scene);

    game_time_t gt;
    gt.delta   = 0.0f;
    gt.elapsed = 0.0f;
    gt.total   = 0.0f;

    clock_t start;
    clock_t diff;
    double frame_delay = 0.0;
    double fps_delay   = 250.0;
    double frame_elap  = 0.0;
    double fps_elap    = 0.0;
    long frames        = 0;
    char fps_buffer[10];
    char title_buffer[30];

    vec2f_t fps_pos = { 0, 0 };
    sprite_text_t fps_disp;
    sprite_text_init(&fps_disp, g_fnt_small, "0.00");
    fps_disp.fast = true;
    sprite_text_set_pos(&fps_disp, fps_pos);

    SDL_Event ev;

    while (g_running)
    {
        start = clock();
    
        SDL_PollEvent(&ev);
        if (SDL_QUIT == ev.type)
        {
            break;
        }

        if (NULL != g_cur_scene)
        {
            g_cur_scene->update(g_cur_scene, &ev, &gt);
        }

        if (!g_cap_fps || frame_delay <= frame_elap)
        {
            ++frames;
            frame_elap = 0.0;

            SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 255); 
            SDL_RenderClear(g_renderer);

            if (g_show_fps)
            {
                sprite_text_render(&fps_disp);
            }

            if (NULL != g_cur_scene)
            {
                g_cur_scene->render(g_cur_scene);
            }

            SDL_RenderPresent(g_renderer);
        }

        fps_elap += gt.elapsed;
        if (fps_delay <= fps_elap)
        {
            gt.fps = (frames / fps_elap) * 1000.0;
            frames = 0;
            fps_elap = 0.0;

            if (g_show_fps)
            {
                snprintf(fps_buffer, sizeof(fps_buffer), "%.2f", gt.fps);
                sprite_text_set_text(&fps_disp, fps_buffer);
            }
            
            snprintf(title_buffer, sizeof(title_buffer), "Quapong - %.2f", gt.fps);
            SDL_SetWindowTitle(g_window, title_buffer);
        }

        diff = clock() - start;
        
        gt.elapsed = (diff * 1000.0) / CLOCKS_PER_SEC;
        gt.total += gt.elapsed;
        gt.delta = (gt.elapsed / frame_delay);

        frame_elap += gt.elapsed;
        frame_delay = 1000.0 / g_max_fps;
    }

    sprite_text_cleanup(&fps_disp);

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
