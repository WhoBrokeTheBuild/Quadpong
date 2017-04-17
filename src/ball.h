#ifndef QUADPONG_BALL_H
#define QUADPONG_BALL_H

#include "quadpong.h"
#include "object.h"

typedef struct ball
{
	object_t base;

    int _last_hit;
}
ball_t;

void ball_init(ball_t * ball);
void ball_cleanup(ball_t * ball);

inline color_t ball_get_color(ball_t * ball) { return object_get_color(&ball->base); }
inline void ball_set_color(ball_t * ball, color_t color) { object_set_color(&ball->base, color); }

inline vec2f_t ball_get_vel(ball_t * ball) { return object_get_vel(&ball->base); }
inline void ball_set_vel(ball_t * ball, vec2f_t vel) { object_set_vel(&ball->base, vel); }

inline vec2f_t ball_get_pos(ball_t * ball) { return object_get_pos(&ball->base); }
inline void ball_set_pos(ball_t * ball, vec2f_t pos) { object_set_pos(&ball->base, pos); }

inline rect_t ball_get_bounding_box(ball_t * ball) { return object_get_bounding_box(&ball->base); }

void ball_update(ball_t * ball, player_t ** players, game_time_t * gt);
void ball_render(ball_t * ball);

#endif // QUADPONG_BALL_H
