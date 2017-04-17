#ifndef QUADPONG_UTIL_H
#define QUADPONG_UTIL_H

#include "quadpong.h"

SDL_Texture *image_load(const char *filename, SDL_Renderer *ren);

#define DEG(RAD) ((RAD) * (180.0 / M_PI))
#define RAD(DEG) ((DEG) * (M_PI / 180.0))

#endif // QUADPONG_UTIL_H
