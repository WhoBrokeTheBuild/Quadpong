#include "game_scene.h"
#include "ball.h"
#include "player.h"

#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>

area_t area_opts[] =
{
    AREA_LEFT,
    AREA_RIGHT,
    AREA_TOP,
    AREA_BOTTOM
};

SDL_Keycode key_opts[][2] =
{
    { SDLK_s, SDLK_w },
    { SDLK_DOWN, SDLK_UP },
    { SDLK_d, SDLK_a },
    { SDLK_RIGHT, SDLK_LEFT },
};

SDL_Color color_opts[] = {
    { 255,   0,   0, 255 },
    {   0, 255,   0, 255 },
    {   0,   0, 255, 255 },
    { 255,   0, 255, 255 }
};

void game_scene_init_base(game_scene_t * scn)
{
    assert(NULL != scn);

    scene_t * base = (scene_t *)scn;
    scene_init(base);

    base->start = &game_scene_start_cb;
    base->stop = &game_scene_stop_cb;
    base->cleanup = &game_scene_cleanup_cb;
    base->update = &game_scene_update_cb;
    base->render = &game_scene_render_cb;

    scn->ball = (ball_t *)malloc(sizeof(ball_t));
    ball_init(scn->ball);

    memset(scn->players, 0, sizeof(player_t *) * MAX_PLAYERS);

    scn->host = false;
    scn->socket = 0;

    vec2f_t msg_pos = { (WIN_WIDTH / 2) - 50, 200 };
    sprite_text_init(&scn->message, g_fnt_large, "Message");
    sprite_text_set_pos(&scn->message, msg_pos);
}

void game_scene_init_local(game_scene_t * scn, int num_players)
{
    game_scene_init_base(scn);

    scn->num_players = num_players;

    for (int i = 0; i < scn->num_players; ++i)
    {
        scn->players[i] = (player_t *)malloc(sizeof(local_player_t));
        local_player_init((local_player_t *)scn->players[i],
            area_opts[i], color_opts[i], key_opts[i][0], key_opts[i][1]);
    }

    scn->state = GAME_STATE_STARTING;
    scn->start_delay = GAME_START_DELAY;
}

void game_scene_init_host(game_scene_t * scn, int num_players)
{
    game_scene_init_base(scn);

    scn->num_players = num_players;

    scn->host = true;
    scn->socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (0 > scn->socket)
    {
        fprintf(stderr, "Failed to create socket\n");
        goto error_socket;
    }

    int optval = 1;
    setsockopt(scn->socket, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons((unsigned short)PORT);

    if (0 > bind(scn->socket, (struct sockaddr *)&addr, sizeof(addr)))
    {
        fprintf(stderr, "Failed to bind socket\n");
        goto error_socket;
    }

    scn->players[0] = (player_t *)malloc(sizeof(local_player_t));
    local_player_init((local_player_t *)scn->players[0],
        area_opts[0], color_opts[0], key_opts[0][0], key_opts[0][1]);

    scn->state = GAME_STATE_WAITING_FOR_PLAYERS;

    return;

error_socket:

    close(scn->socket);
    scn->socket = 0;

    scene_pop();
}

void game_scene_init_connect(game_scene_t * scn, const char * server)
{
    game_scene_init_base(scn);

    scn->host = false;

    scn->state = GAME_STATE_CONNECTING;
}

void game_scene_cleanup_cb(scene_t * scn)
{
    game_scene_t * gscn = (game_scene_t *)scn;

    close(gscn->socket);

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

void game_scene_start_cb(scene_t * scn)
{
    game_scene_t * gscn = (game_scene_t *)scn;

    if (GAME_STATE_PAUSED == gscn->state)
    {
        gscn->state = GAME_STATE_PLAYING;
    }
}

void game_scene_stop_cb(scene_t * scn)
{
    game_scene_t * gscn = (game_scene_t *)scn;

    if (GAME_STATE_PLAYING == gscn->state)
    {
        gscn->state = GAME_STATE_PAUSED;
    }
}

void game_scene_update_cb(scene_t * scn, SDL_Event * ev, game_time_t * gt)
{
    game_scene_t * gscn = (game_scene_t *)scn;

    if (gscn->host)
    {
        fd_set fds;
        struct sockaddr_in addr;
        socklen_t addrlen;
        char buffer[4096];
        struct timeval tv;
        tv.tv_sec = tv.tv_usec = 0;

        FD_ZERO(&fds);
        FD_SET(gscn->socket, &fds);

        if (0 > select(gscn->socket + 1, &fds, NULL, NULL, &tv))
        {
            fprintf(stderr, "Socket closed unexpectedly\n");
            scene_pop();
            return;
        }

        if (FD_ISSET(gscn->socket, &fds))
        {
            addrlen = sizeof(addr);
            int n = recvfrom(gscn->socket, buffer, sizeof(buffer), 0, (struct sockaddr *)&addr, &addrlen);

            if (0 > n)
            {
                fprintf(stderr, "Failed to read data from socket\n");
            }
            else
            {
                printf("%s\n", buffer);
            }
        }
    }

    if (GAME_STATE_STARTING == gscn->state)
    {
        int secs_left = (int)gscn->start_delay / 1000;
        gscn->start_delay -= gt->elapsed;
        if (secs_left > (int)gscn->start_delay / 1000)
        {
            snprintf(gscn->message_buf, sizeof(gscn->message_buf), "%d", secs_left);
            sprite_text_set_text(&gscn->message, gscn->message_buf);
        }

        if (0 >= gscn->start_delay)
        {
            gscn->state = GAME_STATE_PLAYING;
        }
    }
    else if (gscn->state == GAME_STATE_PLAYING)
    {
        for (int i = 0; i < MAX_PLAYERS; ++i)
        {
            if (NULL != gscn->players[i])
            {
                if (gscn->players[i]->update)
                {
                    gscn->players[i]->update(gscn->players[i], ev, gt);
                }
            }
        }

        ball_update(gscn->ball, gscn->players, gt);
    }
}

void game_scene_render_cb(scene_t * scn)
{
    game_scene_t * gscn = (game_scene_t *)scn;

    if (gscn->state != GAME_STATE_PLAYING)
    {
        sprite_text_render(&gscn->message);
    }

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
