#include "game_scene.h"
#include "ball.h"
#include "game_time.h"
#include "player.h"

area_t area_opts[] = {AREA_LEFT, AREA_RIGHT, AREA_TOP, AREA_BOTTOM};

SDL_Keycode key_opts[][2] = {
    {SDLK_s, SDLK_w}, {SDLK_DOWN, SDLK_UP}, {SDLK_d, SDLK_a}, {SDLK_RIGHT, SDLK_LEFT},
};

SDL_Color color_opts[] = {{255, 0, 0, 255}, {0, 255, 0, 255}, {0, 0, 255, 255}, {255, 0, 255, 255}};

void
game_scene_init_base(game_scene_t * gscn)
{
    assert(NULL != gscn);

    scene_t * scn = (scene_t *)gscn;
    scene_init(scn);

    scn->start   = &game_scene_start_cb;
    scn->stop    = &game_scene_stop_cb;
    scn->cleanup = &game_scene_cleanup_cb;
    scn->update  = &game_scene_update_cb;
    scn->render  = &game_scene_render_cb;

    gscn->ball = (ball_t *)malloc(sizeof(ball_t));
    ball_init(gscn->ball);

    memset(gscn->players, 0, sizeof(player_t *) * MAX_PLAYERS);

    gscn->host         = false;
    gscn->socket       = 0;
    gscn->conn_players = 0;

    vec2f_t msg_pos = {(WIN_WIDTH / 2), 150};
    sprite_text_init(&gscn->message, g_fnt_large, " ");
    sprite_text_set_align(&gscn->message, ALIGN_CENTER);
    sprite_text_set_pos(&gscn->message, msg_pos);

    flatcc_builder_init(&gscn->fbs);
}

void
game_scene_init_local(game_scene_t * gscn, int num_players)
{
    game_scene_init_base(gscn);

    gscn->num_players  = num_players;
    gscn->conn_players = gscn->num_players;

    for (int i = 0; i < gscn->num_players; ++i)
    {
        gscn->players[i] = (player_t *)malloc(sizeof(local_player_t));
        local_player_init((local_player_t *)gscn->players[i], area_opts[i], color_opts[i],
                          key_opts[area_opts[i]][0], key_opts[area_opts[i]][1]);
    }

    gscn->state       = GAME_STATE_STARTING;
    gscn->start_delay = GAME_START_DELAY;
    game_scene_update_message(gscn);
}

void
game_scene_init_host(game_scene_t * gscn, int num_players)
{
    game_scene_init_base(gscn);

    int              err;
    struct addrinfo  hints;
    struct addrinfo *result = NULL, *rptr = NULL;

    gscn->num_players  = num_players;
    gscn->conn_players = 1;

    gscn->host = true;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family    = AF_UNSPEC;
    hints.ai_socktype  = SOCK_DGRAM;
    hints.ai_flags     = AI_PASSIVE;
    hints.ai_protocol  = 0;
    hints.ai_canonname = NULL;
    hints.ai_addr      = NULL;
    hints.ai_next      = NULL;

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
    setsockopt(gscn->socket, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

    gscn->players[0] = (player_t *)malloc(sizeof(local_player_t));
    local_player_init((local_player_t *)gscn->players[0], area_opts[0], color_opts[0],
                      key_opts[area_opts[0]][0], key_opts[area_opts[0]][1]);

    gscn->send_update_delay = 0;
    gscn->state             = GAME_STATE_WAITING_FOR_PLAYERS;
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

void
game_scene_init_connect(game_scene_t * gscn, const char * hostname)
{
    game_scene_init_base(gscn);

    int              err;
    struct addrinfo  hints;
    struct addrinfo *result = NULL, *rptr = NULL;

    gscn->host         = false;
    gscn->num_players  = 0;
    gscn->conn_players = 0;

    gscn->send_update_delay = 0;
    gscn->state             = GAME_STATE_CONNECTING;
    game_scene_update_message(gscn);

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags    = 0;
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

    flatcc_builder_reset(&gscn->fbs);
    fbs_join_packet_start_as_root(&gscn->fbs);
    // TODO: ?
    fbs_join_packet_end_as_root(&gscn->fbs);

    size_t    size;
    uint8_t * buf = flatcc_builder_finalize_aligned_buffer(&gscn->fbs, &size);

    printf("Sending join packet (%d)\n", (int)size);
    if ((ssize_t)size > send(gscn->socket, buf, size, 0))
    {
        fprintf(stderr, "Failed to send data to server\n");
        aligned_free(buf);
        goto error_socket;
    }

    aligned_free(buf);

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

void
game_scene_cleanup_cb(scene_t * scn)
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

void
game_scene_start_cb(scene_t * scn)
{
    game_scene_t * gscn = (game_scene_t *)scn;

    if (GAME_STATE_PAUSED == gscn->state)
    {
        gscn->state = GAME_STATE_PLAYING;
    }
}

void
game_scene_stop_cb(scene_t * scn)
{
    game_scene_t * gscn = (game_scene_t *)scn;

    if (GAME_STATE_PLAYING == gscn->state)
    {
        gscn->state = GAME_STATE_PAUSED;
    }
}

void
game_scene_update_cb(scene_t * scn, SDL_Event * ev, game_time_t * gt)
{
    game_scene_t * gscn = (game_scene_t *)scn;

    if (0 != gscn->socket)
    {
        fd_set         fds;
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

        if (gscn->host || GAME_STATE_PLAYING == gscn->state ||
            GAME_STATE_WAITING_FOR_PLAYERS == gscn->state)
        {
            gscn->send_update_delay -= gt->elapsed;
            if (0 >= gscn->send_update_delay)
            {
                gscn->send_update_delay = GAME_SEND_UPDATE_DELAY;
                game_scene_send_update(gscn);
            }
        }

        if (gscn->host)
        {
            for (int i = 1; i < gscn->conn_players; ++i)
            {
                player_t *         ply  = gscn->players[i];
                network_player_t * nply = (network_player_t *)ply;

                nply->ttl -= gt->elapsed;
                if (0 >= nply->ttl)
                {
                    fprintf(stderr, "Player #%d disconnected\n", i);
                    scene_pop();
                    return;
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

void
game_scene_render_cb(scene_t * scn)
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

void
game_scene_update_message(game_scene_t * gscn)
{
    gscn->message_buf[0] = ' ';
    gscn->message_buf[1] = '\0';

    if (GAME_STATE_WAITING_FOR_PLAYERS == gscn->state)
    {
        snprintf(gscn->message_buf, sizeof(gscn->message_buf), "Waiting for %d players",
                 (gscn->num_players - gscn->conn_players));
    }
    else if (GAME_STATE_CONNECTING == gscn->state)
    {
        strncpy(gscn->message_buf, "Connecting", sizeof(gscn->message_buf));
    }
    else if (GAME_STATE_STARTING == gscn->state)
    {
        snprintf(gscn->message_buf, sizeof(gscn->message_buf), "Starting in %d",
                 ((int)gscn->start_delay / 1000));
    }

    sprite_text_set_text(&gscn->message, gscn->message_buf);
}

void
game_scene_handle_packet(game_scene_t * gscn)
{
    int                ret;
    int                size;
    char               buffer[50000];
    struct sockaddr_in addr;
    socklen_t          addrlen;
    player_t *         ply  = NULL;
    network_player_t * nply = NULL;
    size_t             ret_size;
    uint8_t *          ret_buffer = NULL;
    vec2f_t            pos, vel;
    color_t            color;

    memset(buffer, 0, sizeof(buffer));
    addrlen = sizeof(addr);

    size = recvfrom(gscn->socket, buffer, sizeof(buffer), 0, (struct sockaddr *)&addr, &addrlen);
    printf("Recevied %d bytes\n", size);

    if (0 > size)
    {
        fprintf(stderr, "Failed to read data from socket\n");
        scene_pop();
        return;
    }

    if (gscn->host)
    {
        network_player_t * tmp = NULL;
        for (int i = 1; i < gscn->conn_players; ++i)
        {
            tmp = (network_player_t *)gscn->players[i];
            if (0 == sockaddr_cmp((struct sockaddr *)&addr, (struct sockaddr *)&tmp->addr))
            {
                printf("Packet from Player #%d\n", i);
                nply = tmp;
                break;
            }
        }
        if (NULL != nply)
        {
            nply->ttl = MAX_PLAYER_TTL;
        }

        if (GAME_STATE_WAITING_FOR_PLAYERS == gscn->state)
        {
            if (NULL != nply)
            {
                return;
            }

            int player_ind = gscn->conn_players;
            ++gscn->conn_players;

            printf("Player #%d joining\n", player_ind);

            ply                       = (player_t *)malloc(sizeof(network_player_t));
            nply                      = (network_player_t *)ply;
            gscn->players[player_ind] = ply;
            network_player_init(nply, area_opts[player_ind], color_opts[player_ind], &addr,
                                addrlen);

            nply->ttl = MAX_PLAYER_TTL;

            if (gscn->conn_players == gscn->num_players)
            {
                gscn->state = GAME_STATE_STARTING;
            }

            flatcc_builder_reset(&gscn->fbs);
            fbs_welcome_packet_start_as_root(&gscn->fbs);

            fbs_welcome_packet_num_players_add(&gscn->fbs, gscn->num_players);

            pos   = player_get_pos(ply);
            vel   = player_get_vel(ply);
            color = player_get_color(ply);

            fbs_welcome_packet_player_create(&gscn->fbs, pos.x, pos.y, vel.x, vel.y, color.r,
                                             color.g, color.b,
                                             (fbs_area_enum_t)player_get_area(ply));

            fbs_welcome_packet_end_as_root(&gscn->fbs);
            ret_buffer = flatcc_builder_finalize_aligned_buffer(&gscn->fbs, &ret_size);

            printf("Sending welcome packet to #%d (%d)\n", player_ind, (int)size);
            if ((ssize_t)ret_size >
                sendto(gscn->socket, ret_buffer, ret_size, 0, (struct sockaddr *)&addr, addrlen))
            {
                fprintf(stderr, "Failed to send data to client\n");
                aligned_free(ret_buffer);
                return;
            }

            aligned_free(ret_buffer);
        }
        else if (GAME_STATE_PLAYING == gscn->state)
        {
        }
    }
    else
    {
        if (GAME_STATE_CONNECTING == gscn->state)
        {
            fbs_welcome_packet_table_t packet;
            fbs_player_struct_t        player;

            if (0 != (ret = fbs_welcome_packet_verify_as_root(buffer, size)))
            {
                fprintf(stderr, "Failed to verify welcome packet: %s\n", flatcc_verify_error_string(ret));
                return;
            }
            printf("Received welcome packet\n");

            packet = fbs_welcome_packet_as_root(buffer);
            player = fbs_welcome_packet_player(packet);

            gscn->num_players     = fbs_welcome_packet_num_players(packet);
            gscn->players[0]      = (player_t *)malloc(sizeof(local_player_t));
            local_player_t * lply = (local_player_t *)gscn->players[0];

            fbs_area_enum_t    area  = fbs_player_area(player);
            fbs_color_struct_t color = fbs_player_color(player);
            fbs_vec2f_struct_t pos   = fbs_player_pos(player);
            fbs_vec2f_struct_t vel   = fbs_player_vel(player);

            color_t tmp_color = (color_t){color->r, color->g, color->b, 255};

            local_player_init(lply, (area_t)area, tmp_color, key_opts[area][0], key_opts[area][1]);

            area_t new_area = (area_t)0;
            network_player_t * nply = NULL;
            for (int i = 1; i < gscn->num_players; ++i)
            {
                if (new_area == area) new_area++;
                gscn->players[i] = (player_t *)malloc(sizeof(network_player_t));
                nply = (network_player_t *)gscn->players[i];
                network_player_init(nply, new_area, color_opts[new_area], NULL, 0);
                new_area++;
            }

            gscn->state = GAME_STATE_WAITING_FOR_PLAYERS;
            game_scene_update_message(gscn);
        }
        else if (GAME_STATE_WAITING_FOR_PLAYERS == gscn->state)
        {
            fbs_server_update_packet_table_t packet;

            if (0 != (ret = fbs_server_update_packet_verify_as_root(buffer, size)))
            {
                fprintf(stderr, "Failed to verify packet: %s\n", flatcc_verify_error_string(ret));
            }

            printf("Received server update packet\n");
            packet = fbs_server_update_packet_as_root(buffer);

            player_t * ply = NULL;
            fbs_player_vec_t players = fbs_server_update_packet_players(packet);
            if (gscn->conn_players != fbs_player_vec_len(players))
            {
                gscn->conn_players = fbs_player_vec_len(players);
                game_scene_update_message(gscn);
            }

            for (int i = 0; i < (int)fbs_player_vec_len(players); ++i)
            {
                fbs_player_struct_t player = fbs_player_vec_at(players, i);
                ply = NULL;
                for (int i = 1; i < gscn->num_players; ++i)
                {
                    if (fbs_player_area(player) == player_get_area(gscn->players[i]))
                    {
                        ply = gscn->players[i];
                    }
                }
                if (NULL == ply) continue;

                fbs_vec2f_struct_t pos = fbs_player_pos(player);
                fbs_vec2f_struct_t vel = fbs_player_vel(player);
                fbs_color_struct_t color = fbs_player_color(player);

                player_set_pos(ply, (vec2f_t) { pos->x, pos->y });
                player_set_vel(ply, (vec2f_t) { vel->x, vel->y });
                player_set_color(ply, (color_t) { color->r, color->g, color->b, 255 });
            }

            if (gscn->conn_players == gscn->num_players)
            {
                gscn->state = GAME_STATE_STARTING;
            }
        }
        else if (GAME_STATE_PLAYING)
        {
            fbs_server_update_packet_table_t packet;

            if (0 != (ret = fbs_server_update_packet_verify_as_root(buffer, size)))
            {
                fprintf(stderr, "Failed to verify packet: %s\n", flatcc_verify_error_string(ret));
            }

            packet = fbs_server_update_packet_as_root(buffer);
        }
    }
}

void
game_scene_send_update(game_scene_t * gscn)
{
    size_t    size;
    uint8_t * buf = NULL;

    if (gscn->host)
    {
        //printf("Sending update packets\n");

        flatcc_builder_reset(&gscn->fbs);
        fbs_server_update_packet_start_as_root(&gscn->fbs);
        fbs_server_update_packet_state_add(&gscn->fbs, (fbs_game_state_enum_t)gscn->state);

        vec2f_t pos   = ball_get_pos(gscn->ball);
        vec2f_t vel   = ball_get_vel(gscn->ball);
        color_t color = ball_get_color(gscn->ball);
        fbs_server_update_packet_ball_create(&gscn->fbs, pos.x, pos.y, vel.x, vel.y, color.r,
                                             color.g, color.b);

        for (int i = 1; i < gscn->conn_players; ++i)
        {
            player_t *         ply  = gscn->players[i];
            network_player_t * nply = (network_player_t *)ply;

            pos   = player_get_pos(ply);
            vel   = player_get_vel(ply);
            color = player_get_color(ply);

            fbs_server_update_packet_players_push_create(&gscn->fbs, pos.x, pos.y, vel.x, vel.y,
                                                         color.r, color.g, color.b,
                                                         (fbs_area_enum_t)player_get_area(ply));
        }

        fbs_server_update_packet_end_as_root(&gscn->fbs);
        buf = flatcc_builder_finalize_aligned_buffer(&gscn->fbs, &size);

        for (int i = 1; i < gscn->conn_players; ++i)
        {
            player_t *         ply  = gscn->players[i];
            network_player_t * nply = (network_player_t *)ply;

            if ((ssize_t)size > sendto(gscn->socket, buf, size, 0, &nply->addr, sizeof(nply->addr)))
            {
                fprintf(stderr, "Failed to send data to client\n");
                continue;
            }
        }

        aligned_free(buf);
    }
    else
    {
        //printf("Sending update packet\n");

        flatcc_builder_reset(&gscn->fbs);
        fbs_client_update_packet_start_as_root(&gscn->fbs);

        player_t *         ply  = gscn->players[0];
        network_player_t * nply = (network_player_t *)ply;

        vec2f_t pos   = player_get_pos(ply);
        vec2f_t vel   = player_get_vel(ply);
        color_t color = player_get_color(ply);

        fbs_client_update_packet_player_create(&gscn->fbs, pos.x, pos.y, vel.x, vel.y, color.r,
                                               color.g, color.b, player_get_area(ply));

        fbs_client_update_packet_end_as_root(&gscn->fbs);
        buf = flatcc_builder_finalize_aligned_buffer(&gscn->fbs, &size);

        if ((ssize_t)size > send(gscn->socket, buf, size, 0))
        {
            fprintf(stderr, "Failed to send data to server\n");
            aligned_free(buf);
            return;
        }

        aligned_free(buf);
    }
}
