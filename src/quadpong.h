#ifndef QUADPONG_H
#define QUADPONG_H

#define _MATH_DEFINES_DEFINED

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

#ifdef WIN32

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <ws2tcpip.h>

#include <BaseTsd.h>
typedef SSIZE_T ssize_t;

#else 

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#endif // WIN32

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

typedef SDL_Rect  rect_t;
typedef SDL_Color color_t;

typedef enum align align_t;

typedef struct vec2       vec2_t;
typedef struct vec2f      vec2f_t;
typedef struct sprite     sprite_t;
typedef struct object	  object_t;
typedef struct ball		  ball_t;
typedef struct player     player_t;
typedef struct scene      scene_t;
typedef struct menu_scene menu_scene_t;
typedef struct game_scene game_scene_t;
typedef struct game_time  game_time_t;

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

#define PLAYER_SPEED        (5.0f)
#define BALL_SPEED			(5.0f)

#define PLAYER_V_WIDTH      (32)
#define PLAYER_V_HEIGHT     (128)
#define PLAYER_H_WIDTH      (128)
#define PLAYER_H_HEIGHT     (32)

#define BALL_SIZE           (16)
#define WALL_PADDING		(PLAYER_V_WIDTH / 2)
#define CORNER_SIZE         (PLAYER_V_WIDTH + WALL_PADDING)

#define MAX_PLAYERS         (4)

#define PORT                "57664"
#define UPDATE_DELAY        (60)
#define MAX_PLAYER_TTL      (UPDATE_DELAY * 4)

#define ASSET_PATH          "assets/"
#define PIXEL_ASSET_PATH    ASSET_PATH "pixel.png"
#define TITLE_ASSET_PATH    ASSET_PATH "title.png"
#define FONT_ASSET_PATH     ASSET_PATH "roboto.ttf"

#endif // QUADPONG_H
