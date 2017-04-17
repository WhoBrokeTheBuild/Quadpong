#ifndef QUADPONG_VEC2_H
#define QUADPONG_VEC2_H

#include "quadpong.h"

typedef struct vec2
{
    int x;
    int y;
} vec2_t;

typedef struct vec2f
{
    float x;
    float y;
} vec2f_t;

static inline vec2_t
vec2_add(vec2_t a, vec2_t b)
{
    return (vec2_t){a.x + b.x, a.y + b.y};
}

static inline vec2_t
vec2_muls(vec2_t a, int c)
{
    return (vec2_t){a.x * c, a.y * c};
}

static inline vec2f_t
vec2f_add(vec2f_t a, vec2f_t b)
{
    return (vec2f_t){a.x + b.x, a.y + b.y};
}

static inline vec2f_t
vec2f_muls(vec2f_t a, float c)
{
    return (vec2f_t){a.x * c, a.y * c};
}

#endif // QUADPONG_VEC2_H
