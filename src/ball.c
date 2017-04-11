#include "ball.h"
#include "player.h"

void ball_init(ball_t * ball)
{
    ball->sprite = (sprite_t *)malloc(sizeof(sprite_t));
    sprite_load(ball->sprite, PIXEL_ASSET_PATH);

    int angle = rand() % 360;
    ball->vel.x = cosf(angle) * 5.0f;
    ball->vel.y = sinf(angle) * 5.0f;

    vec2_t pos = { (WIN_WIDTH / 2) - (BALL_SIZE / 2), (WIN_HEIGHT / 2) - (BALL_SIZE / 2) };
    vec2_t size = { BALL_SIZE, BALL_SIZE };
    sprite_set_pos(ball->sprite, pos);
    sprite_set_size(ball->sprite, size);
}

void ball_update(ball_t * ball, struct player ** players)
{
    vec2_t pos = sprite_get_pos(ball->sprite);

    int p_left = -1,
        p_right = -1,
        p_top = -1,
        p_bot = -1;

    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
        if (SDL_HasIntersection(&(ball->sprite->dst_rect), &(players[i]->sprite->dst_rect)))
        {
            if (AREA_LEFT == players[i]->area || AREA_RIGHT == players[i]->area)
            {
                ball->vel.x = -ball->vel.x;
            }
            else
            {
                ball->vel.y = -ball->vel.y;
            }

            break;
        }

        if (AREA_LEFT == players[i]->area)
        {
            p_left = i;
        }
        else if (AREA_RIGHT == players[i]->area)
        {
            p_right = i;
        }
        else if (AREA_TOP == players[i]->area)
        {
            p_top = i;
        }
        else if (AREA_BOTTOM == players[i]->area)
        {
            p_bot = i;
        }
    }

    if (0 > pos.x)
    {
        if (0 >= p_left)
        {
            printf("Player %d loses", p_left);
            exit(0); // TODO: Improve
        }
        else
        {
            ball->vel.x = -ball->vel.x;
        }
    }
    else if (0 > pos.y)
    {
        if (0 >= p_top)
        {
            printf("Player %d loses", p_top);
            exit(0); // TODO: Improve
        }
        else
        {
            ball->vel.y = -ball->vel.y;
        }
    }
    else if (WIN_WIDTH < pos.x)
    {
        if (0 >= p_right)
        {
            printf("Player %d loses", p_right);
            exit(0); // TODO: Improve
        }
        else
        {
            ball->vel.x = -ball->vel.x;
        }
    }
    else if (WIN_HEIGHT < pos.y)
    {
        if (0 >= p_bot)
        {
            printf("Player %d loses", p_bot);
            exit(0); // TODO: Improve
        }
        else
        {
            ball->vel.y = -ball->vel.y;
        }
    }

    pos.x += ball->vel.x;
    pos.y += ball->vel.y;
    sprite_set_pos(ball->sprite, pos);
}

void ball_render(ball_t * ball)
{
    sprite_render(ball->sprite);
}
