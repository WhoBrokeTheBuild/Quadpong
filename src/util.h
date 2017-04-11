#ifndef QUAPONG_UTIL_H
#define QUAPONG_UTIL_H

#include "quapong.h"

SDL_Texture * image_load(const char * filename, SDL_Renderer * ren);

#define DEG(RAD) ((RAD) * (180.0 / M_PI))
#define RAD(DEG) ((DEG) * (M_PI / 180.0))

#endif // QUAPONG_UTIL_H
