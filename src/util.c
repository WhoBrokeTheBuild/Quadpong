#include "util.h"

SDL_Texture * image_load(const char * filename, SDL_Renderer * ren)
{
    SDL_Surface * surf;
    SDL_Texture * tex;

    printf("Loading texture %s\n", filename);

    surf = IMG_Load(filename);
    if (NULL == surf)
    {
        fprintf(stderr, "SDL_LoadIMG failed: %s\n", SDL_GetError());
        tex = NULL;
        goto error;
    }

    tex = SDL_CreateTextureFromSurface(ren, surf);
    if (NULL == tex)
    {
        fprintf(stderr, "SDL_CreateTextureFromSurface failed: %s\n", SDL_GetError());
        tex = NULL;
        goto error;
    }

    printf("Finished loading %s\n", filename);

error:

    SDL_FreeSurface(surf);

    return tex;
}

