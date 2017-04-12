#ifndef QUADPONG_H
#define QUADPONG_H

#define _MATH_DEFINES_DEFINED 

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

extern SDL_Window * g_window;
extern SDL_Renderer * g_renderer;

extern TTF_Font * g_fnt_large;
extern TTF_Font * g_fnt_small;

extern bool g_running;
extern bool g_cap_fps;
extern bool g_show_fps;
extern int g_max_fps;

#define GAME_TITLE          "Quadpong"

#define WIN_WIDTH           (700)
#define WIN_HEIGHT          (700 + 20)

#define FONT_LARGE_SIZE     (40)
#define FONT_SMALL_SIZE     (20)

#define BALL_SIZE           (16)

#define PLAYER_V_WIDTH      (32)
#define PLAYER_V_HEIGHT     (128)
#define PLAYER_H_WIDTH      (128)
#define PLAYER_H_HEIGHT     (32)

#define CORNER_SIZE         ((PLAYER_V_WIDTH + (PLAYER_V_WIDTH / 2)))

#define MAX_PLAYERS         (4)

#define PORT                "57664"

#define ASSET_PATH          "assets/"
#define PIXEL_ASSET_PATH    ASSET_PATH "pixel.png"
#define TITLE_ASSET_PATH    ASSET_PATH "title.png"
#define FONT_ASSET_PATH     ASSET_PATH "roboto.ttf"

#endif // QUADPONG_H
