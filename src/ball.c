#include "ball.h"
#include "player.h"
#include "scene.h"

void ball_init(ball_t * ball)
{
    ball->sprite = (sprite_t *)malloc(sizeof(sprite_t));
    sprite_init(ball->sprite, PIXEL_ASSET_PATH);

    ball->last_hit = -1;

    float angle = (float)RAD(rand() % 360);
    ball->vel.x = cosf(angle) * 5.0f;
    ball->vel.y = sinf(angle) * 5.0f;

    vec2f_t pos = { (WIN_WIDTH / 2) - (BALL_SIZE / 2), (WIN_HEIGHT / 2) - (BALL_SIZE / 2) };
    vec2_t size = { BALL_SIZE, BALL_SIZE };
    sprite_set_pos(ball->sprite, pos);
    sprite_set_size(ball->sprite, size);
}

void ball_cleanup(ball_t * ball)
{
    sprite_cleanup(ball->sprite);
}

void ball_update(ball_t * ball, struct player ** players, game_time_t * gt)
{
    vec2f_t tpos = ball->pos;
    tpos.x += ball->vel.x * gt->delta;
    tpos.y += ball->vel.y * gt->delta;

    int p_left = -1,
        p_right = -1,
        p_top = -1,
        p_bot = -1;

    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
        if (NULL == players[i]) break;

        if (SDL_HasIntersection(&(ball->sprite->dst_rect), &(players[i]->sprite->dst_rect)))
        {
            ball->last_hit = i;
            ball->color = players[i]->color;
            SDL_SetTextureColorMod(ball->sprite->texture, ball->color.r, ball->color.g, ball->color.b);

            if (AREA_LEFT == players[i]->area || AREA_RIGHT == players[i]->area)
            {
                ball->vel.x = -ball->vel.x;
            }
            else
            {
                ball->vel.y = -ball->vel.y;
            }
        }

        switch (players[i]->area)
        {
        case AREA_LEFT:     p_left = i;  break;
        case AREA_RIGHT:    p_right = i; break;
        case AREA_TOP:      p_top = i;   break;
        case AREA_BOTTOM:   p_bot = i;   break;
        }
    }

    if (0 >= tpos.x)
    {
        if (-1 != p_left)
        {
            printf("Player %d loses\n", p_left);
            scene_pop();
            return;
        }
        else
        {
            ball->vel.x = -ball->vel.x;
        }
    }
    else if (0 >= tpos.y)
    {
        if (-1 != p_top)
        {
            printf("Player %d loses\n", p_top);
            scene_pop();
            return;
        }
        else
        {
            ball->vel.y = -ball->vel.y;
        }
    }
    else if (WIN_WIDTH <= tpos.x)
    {
        if (-1 != p_right)
        {
            printf("Player %d loses\n", p_right);
            scene_pop();
            return;
        }
        else
        {
            ball->vel.x = -ball->vel.x;
        }
    }
    else if (WIN_HEIGHT <= tpos.y)
    {
        if (-1 != p_bot)
        {
            printf("Player %d loses\n", p_bot);
            scene_pop();
            return;
        }
        else
        {
            ball->vel.y = -ball->vel.y;
        }
    }

    ball->pos.x += ball->vel.x * gt->delta;
    ball->pos.y += ball->vel.y * gt->delta;
    sprite_set_pos(ball->sprite, ball->pos);
}

void ball_render(ball_t * ball)
{
    sprite_render(ball->sprite);
}
