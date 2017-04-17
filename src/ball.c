#include "ball.h"
#include "game_time.h"
#include "player.h"

void ball_init(ball_t * ball)
{
	object_t * obj = &ball->base;
	object_init(obj);

	sprite_create(object_get_sprite(obj), 1, 1, (color_t) { 255, 255, 255 });
	sprite_set_size(object_get_sprite(obj), (vec2_t) { BALL_SIZE, BALL_SIZE });

    ball->_last_hit = -1;

    float angle = (float)RAD(rand() % 360);
	ball_set_vel(ball, (vec2f_t) {
		cosf(angle) * BALL_SPEED, 
		sinf(angle) * BALL_SPEED });

	ball_set_pos(ball, (vec2f_t) {
		(WIN_WIDTH / 2) - (BALL_SIZE / 2),
		(WIN_HEIGHT / 2) - (BALL_SIZE / 2) });
}

void ball_cleanup(ball_t * ball)
{
	object_t * obj = &ball->base;
	object_cleanup(obj);
}

void ball_update(ball_t * ball, struct player ** players, game_time_t * gt)
{
	object_t * obj = &ball->base;

	vec2f_t vel = ball_get_vel(ball);
    vec2f_t tpos = vec2f_add(ball_get_pos(ball), 
							 vec2f_muls(vel, gt->delta));

    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
        if (NULL == players[i]) break;

		rect_t ball_rect = ball_get_bounding_box(ball);
		rect_t player_rect = player_get_bounding_box(players[i]);
        if (SDL_HasIntersection(&ball_rect, &player_rect))
        {
            ball->_last_hit = i;
			ball_set_color(ball, player_get_color(players[i]));

            if (AREA_LEFT == players[i]->_area || AREA_RIGHT == players[i]->_area)
            {
                vel.x = -vel.x;
            }
            else
            {
                vel.y = -vel.y;
            }
        }
    }

    if (0 >= tpos.x && vel.x < 0.0f)
	{
		vel.x = -vel.x;
    }
    else if (0 >= tpos.y && vel.y < 0.0f)
    {
        vel.y = -vel.y;
    }
    else if (WIN_WIDTH <= tpos.x && vel.x > 0.0f)
    {
        vel.x = -vel.x;
    }
    else if (WIN_HEIGHT <= tpos.y && vel.y > 0.0f)
    {
        vel.y = -vel.y;
    }

	ball_set_vel(ball, vel);
	object_update(obj, gt);
}

void ball_render(ball_t * ball)
{
	object_t * obj = &ball->base;
	object_render(obj);
}
