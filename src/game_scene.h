#ifndef QUADPONG_GAME_SCENE_H
#define QUADPONG_GAME_SCENE_H

#include "quadpong.h"
#include "scene.h"
#include "sprite.h"
#include <packet_reader.h>
#include <packet_builder.h>

#define GAME_START_DELAY 3000

struct player;
struct ball;
struct sprite_text;

typedef enum game_state
{
    GAME_STATE_WAITING_FOR_PLAYERS = 1,
    GAME_STATE_CONNECTING          = 2,
    GAME_STATE_STARTING            = 3,
    GAME_STATE_PLAYING             = 4,
    GAME_STATE_PAUSED              = 5,
}
game_state_t;

typedef struct game_scene
{
    scene_t base;

    int num_players;
    game_state_t state;
    double start_delay;
    sprite_text_t message;
    char message_buf[100];

    struct player * players[MAX_PLAYERS];
    struct ball *   ball;

    bool host;
 #ifdef WIN32
	SOCKET socket;
#else
    int socket;
#endif
    int conn_players;
    flatcc_builder_t fbs;
}
game_scene_t;

void game_scene_init_base(game_scene_t * gscn);
void game_scene_init_local(game_scene_t * gscn, int num_players);
void game_scene_init_host(game_scene_t * gscn, int num_players);
void game_scene_init_connect(game_scene_t * gscn, const char * server);
void game_scene_cleanup_cb(scene_t * scn);
void game_scene_start_cb(scene_t * scn);
void game_scene_stop_cb(scene_t * scn);
void game_scene_update_cb(scene_t * scn, SDL_Event * ev, game_time_t * gt);
void game_scene_render_cb(scene_t * scn);
void game_scene_update_message(game_scene_t * gscn);
void game_scene_handle_packet(game_scene_t * gscn);
void game_scene_send_updates(game_scene_t * gscn);

#endif // QUADPONG_GAME_SCENE_H
