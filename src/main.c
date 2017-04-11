#include "quapong.h"
#include "sprite.h"
#include "ball.h"
#include "player.h"
#include "util.h"

SDL_Window      * g_window = NULL;
SDL_Renderer    * g_renderer = NULL;

int main(int argc, char ** argv)
{
    int             retval = 0;
    ball_t          ball;
    player_t *      players[MAX_PLAYERS];
    //wall_t *        walls[4]; // TODO: Something?

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

    ball_init(&ball);

    memset(players, 0, sizeof(player_t *) * MAX_PLAYERS);
    
    players[0] = (player_t *)malloc(sizeof(local_player_t));
    local_player_init((local_player_t *)players[0], AREA_LEFT, SDLK_DOWN, SDLK_UP);

    players[1] = (player_t *)malloc(sizeof(local_player_t));
    local_player_init((local_player_t *)players[1], AREA_RIGHT, SDLK_s, SDLK_w);
    
    players[2] = (player_t *)malloc(sizeof(local_player_t));
    local_player_init((local_player_t *)players[2], AREA_TOP, SDLK_RIGHT, SDLK_LEFT);

    players[3] = (player_t *)malloc(sizeof(local_player_t));
    local_player_init((local_player_t *)players[3], AREA_BOTTOM, SDLK_d, SDLK_a);
    
    SDL_Event ev;

    for (;;)
    {
        SDL_PollEvent(&ev);
        if (SDL_QUIT == ev.type)
        {
            break;
        }

        SDL_SetRenderDrawColor(g_renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(g_renderer);
        
        for (int i = 0; i < MAX_PLAYERS; ++i)
        {
            if (NULL != players[i])
            {
                if (players[i]->update)
                {
                    players[i]->update(players[i], &ev);
                }
                if (players[i]->render)
                {
                    players[i]->render(players[i]);
                }
            }
        }

        ball_update(&ball, players);
        ball_render(&ball);
    
        SDL_RenderPresent(g_renderer);

        // TODO: Improve
        SDL_Delay(16);
    }

error_assets:

    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
        if (players[i])
        {
            if (players[i]->cleanup)
            {
                players[i]->cleanup(players[i]);
            }
            free(players[i]);
        }
    }

error_sdl:

    IMG_Quit();
    
    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);
    SDL_Quit();

    return retval;
}
