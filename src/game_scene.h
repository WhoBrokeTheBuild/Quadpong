#ifndef QUAPONG_GAME_SCENE_H
#define QUAPONG_GAME_SCENE_H

#include "quapong.h"
#include "scene.h"
#include "sprite.h"

struct player;
struct ball;
struct sprite_text;

typedef enum game_state
{
    GAME_STATE_WAITING_FOR_PLAYERS,
    GAME_STATE_CONNECTING,
    GAME_STATE_STARTING,
    GAME_STATE_PLAYING,
    GAME_STATE_PAUSED,
}
game_state_t;

typedef struct game_scene
{
    scene_t base;

    bool host;
    int socket;

    int num_players;
    game_state_t state;
    sprite_text_t message;
    int start_delay;

    struct player * players[MAX_PLAYERS];
    struct ball *   ball;
}
game_scene_t;

void game_scene_init_base(game_scene_t * scn);
void game_scene_init_local(game_scene_t * scn, int num_players);
void game_scene_init_host(game_scene_t * scn, int num_players);
void game_scene_init_connect(game_scene_t * scn, const char * server);
void game_scene_cleanup_cb(scene_t * scn);
void game_scene_start_cb(scene_t * scn);
void game_scene_stop_cb(scene_t * scn);
void game_scene_update_cb(scene_t * scn, SDL_Event * ev, float delta);
void game_scene_render_cb(scene_t * scn);

#endif // QUAPONG_GAME_SCENE_H
