#ifndef QUADPONG_BALL_H
#define QUADPONG_BALL_H

#include "quadpong.h"

#include "object.h"

typedef struct player player_t;

/**
 * @brief A ball, used in #game_scene.
 *
 * Structure representing a ball, used in #game_scene.
 * Inherits from #object.
 */
typedef struct ball
{
    object_t _base; /**< Parent struct.*/

    int _last_hit; /**< ID of the player that last hit the ball, or -1. */

} ball_t;

/**
 * Initialize a default ball.
 * @param ball The balll.
 */
void ball_init(ball_t *ball);

/**
 * Cleanup resources used by ball.
 * @param ball The ball.
 */
void ball_cleanup(ball_t *ball);

/**
 * Passthrough to #object_get_color.
 */
static inline color_t ball_get_color(ball_t *ball)
{
    return object_get_color(&ball->_base);
}

/**
 * Passthrough to #object_set_color.
 */
static inline void ball_set_color(ball_t *ball, color_t color)
{
    object_set_color(&ball->_base, color);
}

/**
 * Passthrough to #object_get_vel.
 */
static inline vec2f_t ball_get_vel(ball_t *ball)
{
    return object_get_vel(&ball->_base);
}

/**
 * Passthrough to #object_set_vel.
 */
static inline void ball_set_vel(ball_t *ball, vec2f_t vel)
{
    object_set_vel(&ball->_base, vel);
}

/**
 * Passthrough to #object_get_pos.
 */
static inline vec2f_t ball_get_pos(ball_t *ball)
{
    return object_get_pos(&ball->_base);
}

/**
 * Passthrough to #object_set_pos.
 */
static inline void ball_set_pos(ball_t *ball, vec2f_t pos)
{
    object_set_pos(&ball->_base, pos);
}

/**
 * Passthrough to #object_get_bounding_box.
 */
static inline rect_t ball_get_bounding_box(ball_t *ball)
{
    return object_get_bounding_box(&ball->_base);
}

/**
 * Update ball movement and check collision with players.
 * Calls #object_updates.
*/
void ball_update(ball_t *ball, player_t **players, game_time_t *gt);

/**
 * Passthrough to #object_updates.
 */
static inline void ball_render(ball_t *ball)
{
    object_render(&ball->_base);
}

#endif // QUADPONG_BALL_H
