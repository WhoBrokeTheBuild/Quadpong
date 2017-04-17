#include "game_scene.h"
#include "ball.h"
#include "player.h"

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

void game_scene_init_base(game_scene_t * gscn)
{
    assert(NULL != gscn);

    scene_t * scn = (scene_t *)gscn;
    scene_init(scn);

    scn->start = &game_scene_start_cb;
    scn->stop = &game_scene_stop_cb;
    scn->cleanup = &game_scene_cleanup_cb;
    scn->update = &game_scene_update_cb;
    scn->render = &game_scene_render_cb;

    gscn->ball = (ball_t *)malloc(sizeof(ball_t));
    ball_init(gscn->ball);

    memset(gscn->players, 0, sizeof(player_t *) * MAX_PLAYERS);

    gscn->host = false;
    gscn->socket = 0;
    gscn->conn_players = 0;

    vec2f_t msg_pos = { (WIN_WIDTH / 2), 150 };
    sprite_text_init(&gscn->message, g_fnt_large, " ");
    sprite_text_set_align(&gscn->message, SPRITE_ALIGN_CENTER);
    sprite_text_set_pos(&gscn->message, msg_pos);

    flatcc_builder_init(&gscn->fbs);
}

void game_scene_init_local(game_scene_t * gscn, int num_players)
{
    game_scene_init_base(gscn);

    gscn->num_players = num_players;
    gscn->conn_players = gscn->num_players;

    for (int i = 0; i < gscn->num_players; ++i)
    {
        gscn->players[i] = (player_t *)malloc(sizeof(local_player_t));
        local_player_init((local_player_t *)gscn->players[i],
            area_opts[i], color_opts[i], key_opts[i][0], key_opts[i][1]);
    }

    gscn->state = GAME_STATE_STARTING;
    gscn->start_delay = GAME_START_DELAY;
    game_scene_update_message(gscn);
}

void game_scene_init_host(game_scene_t * gscn, int num_players)
{
    game_scene_init_base(gscn);

    int err;
    struct addrinfo hints;
    struct addrinfo * result = NULL,
                    * rptr = NULL;

    gscn->num_players = num_players;
    gscn->conn_players = 1;

    gscn->host = true;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    if (0 != (err = getaddrinfo(NULL, PORT, &hints, &result)))
    {
        fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(err));
        goto error_socket;
    }

    for (rptr = result; NULL != rptr; rptr = rptr->ai_next)
    {
        gscn->socket = socket(rptr->ai_family, rptr->ai_socktype, rptr->ai_protocol);
        if (0 > gscn->socket)
            continue;

        if (0 == bind(gscn->socket, rptr->ai_addr, (socklen_t)rptr->ai_addrlen))
            break;

#ifdef WIN32
		closesocket(gscn->socket);
#else
		close(gscn->socket);
#endif
    }

    if (NULL == rptr)
    {
        fprintf(stderr, "Failed to bind socket\n");
        goto error_socket;
    }

    freeaddrinfo(result);

    int optval = 1;
    setsockopt(gscn->socket, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));

    gscn->players[0] = (player_t *)malloc(sizeof(local_player_t));
    local_player_init((local_player_t *)gscn->players[0],
        area_opts[0], color_opts[0], key_opts[0][0], key_opts[0][1]);

    gscn->state = GAME_STATE_WAITING_FOR_PLAYERS;
    game_scene_update_message(gscn);

    return;

error_socket:

    freeaddrinfo(result);
#ifdef WIN32
	closesocket(gscn->socket);
#else
	close(gscn->socket);
#endif
    gscn->socket = 0;

    scene_pop();
}

void game_scene_init_connect(game_scene_t * gscn, const char * hostname)
{
    game_scene_init_base(gscn);

    int err;
    struct addrinfo hints;
    struct addrinfo * result = NULL,
                    * rptr = NULL;

    gscn->host = false;
    gscn->num_players = 0;
    gscn->conn_players = 0;

    gscn->state = GAME_STATE_CONNECTING;
    game_scene_update_message(gscn);

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;

    if (0 != (err = getaddrinfo(hostname, PORT, &hints, &result)))
    {
        fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(err));
        goto error_socket;
    }

    for (rptr = result; NULL != rptr; rptr = rptr->ai_next)
    {
        gscn->socket = socket(rptr->ai_family, rptr->ai_socktype, rptr->ai_protocol);
        if (0 > gscn->socket)
            continue;

        if (0 == connect(gscn->socket, rptr->ai_addr, (socklen_t)rptr->ai_addrlen))
            break;

#ifdef WIN32
		closesocket(gscn->socket);
#else
		close(gscn->socket);
#endif
    }

    if (NULL == rptr)
    {
        fprintf(stderr, "Failed to connect to server\n");
        goto error_socket;
    }

    freeaddrinfo(result);

    p_welcome_packet_start_as_root(&gscn->fbs);
    // TODO: ?
    p_welcome_packet_end_as_root(&gscn->fbs);

    size_t size;
    uint8_t * buf = flatcc_builder_finalize_aligned_buffer(&gscn->fbs, &size);

    if ((ssize_t)size > send(gscn->socket, buf, size, 0))
    {
        fprintf(stderr, "Failed to send data to server\n");
        goto error_socket;
    }

    return;

error_socket:

    freeaddrinfo(result);
#ifdef WIN32
	closesocket(gscn->socket);
#else
	close(gscn->socket);
#endif
    gscn->socket = 0;

    scene_pop();
}

void game_scene_cleanup_cb(scene_t * scn)
{
    game_scene_t * gscn = (game_scene_t *)scn;

#ifdef WIN32
	closesocket(gscn->socket);
#else
	close(gscn->socket);
#endif
    flatcc_builder_clear(&gscn->fbs);

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

    if (0 != gscn->socket)
    {
        fd_set fds;
        struct timeval tv;
        tv.tv_sec = tv.tv_usec = 0;

        FD_ZERO(&fds);
        FD_SET(gscn->socket, &fds);

        if (0 > select((int)gscn->socket + 1, &fds, NULL, NULL, &tv))
        {
            fprintf(stderr, "Socket closed unexpectedly\n");
            scene_pop();
            return;
        }

        if (FD_ISSET(gscn->socket, &fds))
        {
            game_scene_handle_packet(gscn);
        }

        if (gscn->host)
        {
            game_scene_send_updates(gscn);

            for (int i = 1; i < gscn->conn_players; ++i)
            {
                player_t * ply = gscn->players[i];
                network_player_t * nply = (network_player_t *)ply;

                nply->ttl -= gt->elapsed;
                if (0 >= nply->ttl)
                {
                    fprintf(stderr, "Player %d disconnected\n", i);
                    scene_pop();
                }
            }
        }
    }

    if (GAME_STATE_STARTING == gscn->state)
    {
        int secs_left = (int)gscn->start_delay / 1000;
        gscn->start_delay -= gt->elapsed;
        if (secs_left > (int)gscn->start_delay / 1000)
        {
            game_scene_update_message(gscn);
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

void game_scene_update_message(game_scene_t * gscn)
{
    gscn->message_buf[0] = ' ';
    gscn->message_buf[1] = '\0';

    if (GAME_STATE_WAITING_FOR_PLAYERS == gscn->state)
    {
        snprintf(gscn->message_buf, sizeof(gscn->message_buf), "Waiting for %d players", (gscn->num_players - gscn->conn_players));
    }
    else if (GAME_STATE_CONNECTING == gscn->state)
    {
        strncpy(gscn->message_buf, "Connecting", sizeof(gscn->message_buf));
    }
    else if (GAME_STATE_STARTING == gscn->state)
    {
        snprintf(gscn->message_buf, sizeof(gscn->message_buf), "Starting in %d", ((int)gscn->start_delay / 1000));
    }

    sprite_text_set_text(&gscn->message, gscn->message_buf);
}

void game_scene_handle_packet(game_scene_t * gscn)
{
    int bytes_read;
    char buffer[4096];
    struct sockaddr_in addr;
    socklen_t addrlen;
    addrlen = sizeof(addr);

    bytes_read = recvfrom(gscn->socket, buffer, sizeof(buffer), 0, (struct sockaddr *)&addr, &addrlen);

    if (0 > bytes_read)
    {
        fprintf(stderr, "Failed to read data from socket\n");
        scene_pop();
        return;
    }

    if (GAME_STATE_CONNECTING == gscn->state)
    {
        gscn->state = GAME_STATE_WAITING_FOR_PLAYERS;
        game_scene_update_message(gscn);
    }
    else if (GAME_STATE_WAITING_FOR_PLAYERS == gscn->state)
    {
        // TODO: Assign to player

        char tmp[] = "pong";
        if ((ssize_t)sizeof(tmp) > sendto(gscn->socket, tmp, sizeof(tmp), 0, (struct sockaddr *)&addr, addrlen))
        {
            fprintf(stderr, "Failed to send data to client\n");
            return;
        }

        int player_ind = gscn->conn_players;
        ++gscn->conn_players;

        gscn->players[player_ind] = (player_t *)malloc(sizeof(network_player_t));
        network_player_t * nply = (network_player_t *)gscn->players[player_ind];
        network_player_init(nply, area_opts[player_ind], color_opts[player_ind], &addr, addrlen);

        ++gscn->conn_players;

        if (gscn->conn_players == gscn->num_players)
        {
            gscn->state = GAME_STATE_STARTING;
        }
    }
}

void game_scene_send_updates(game_scene_t * gscn)
{
    size_t size;
    uint8_t * buf = NULL;

    p_update_packet_start_as_root(&gscn->fbs);
    p_update_packet_state_add(&gscn->fbs, (p_game_state_enum_t)gscn->state);

    // TODO: Move?
    p_update_packet_ball_create(&gscn->fbs,
        gscn->ball->pos.x, gscn->ball->pos.y,
        gscn->ball->vel.x, gscn->ball->vel.y,
        gscn->ball->color.r, gscn->ball->color.g, gscn->ball->color.b);

    for (int i = 1; i < gscn->conn_players; ++i)
    {
        player_t * ply = gscn->players[i];
        network_player_t * nply = (network_player_t *)ply;

        p_update_packet_players_push_create(&gscn->fbs,
            ply->pos.x, ply->pos.y,
            ply->vel.x, ply->vel.y,
            ply->color.r, ply->color.g, ply->color.b,
            (p_area_enum_t)ply->area);
    }

    p_update_packet_end_as_root(&gscn->fbs);

    buf = flatcc_builder_finalize_aligned_buffer(&gscn->fbs, &size);

    for (int i = 1; i < gscn->conn_players; ++i)
    {
        player_t * ply = gscn->players[i];
        network_player_t * nply = (network_player_t *)ply;

        if ((ssize_t)size > sendto(gscn->socket, buf, size, 0, &nply->addr, sizeof(nply->addr)))
        {
            fprintf(stderr, "Failed to send data to client\n");
            continue;
        }

        nply->ttl = MAX_PLAYER_TTL;
    }
}
