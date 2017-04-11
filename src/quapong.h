#ifndef QUAPONG_H
#define QUAPONG_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>

#include <SDL.h>
#include <SDL_image.h>

extern SDL_Window * g_window;
extern SDL_Renderer * g_renderer;

#define WIN_WIDTH           (600)
#define WIN_HEIGHT          (600 + 20)

#define BALL_SIZE           (16)

#define PLAYER_V_WIDTH      (32)
#define PLAYER_V_HEIGHT     (128)
#define PLAYER_H_WIDTH      (128)
#define PLAYER_H_HEIGHT     (32)

#define CORNER_SIZE         ((PLAYER_V_WIDTH + (PLAYER_V_WIDTH / 2)))

#define MAX_PLAYERS         (4)

#define ASSET_PATH          "assets/"
#define PIXEL_ASSET_PATH    ASSET_PATH "pixel.png"

#endif // QUAPONG_H
