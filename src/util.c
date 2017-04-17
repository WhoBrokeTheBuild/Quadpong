#include "util.h"

SDL_Texture *
image_load(const char * filename, SDL_Renderer * ren)
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

/*
 * Copyright (c) 2014 Kazuho Oku
 */
int
sockaddr_cmp(struct sockaddr * x, struct sockaddr * y)
{
#define CMP(a, b) \
    if (a != b)   \
    return a < b ? -1 : 1

    CMP(x->sa_family, y->sa_family);

    if (x->sa_family == AF_INET)
    {
        struct sockaddr_in *xin = (void *)x, *yin = (void *)y;
        CMP(ntohl(xin->sin_addr.s_addr), ntohl(yin->sin_addr.s_addr));
        CMP(ntohs(xin->sin_port), ntohs(yin->sin_port));
    }
    else if (x->sa_family == AF_INET6)
    {
        struct sockaddr_in6 *xin6 = (void *)x, *yin6 = (void *)y;
        int                  r = memcmp(xin6->sin6_addr.s6_addr, yin6->sin6_addr.s6_addr,
                       sizeof(xin6->sin6_addr.s6_addr));
        if (r != 0)
            return r;
        CMP(ntohs(xin6->sin6_port), ntohs(yin6->sin6_port));
        CMP(xin6->sin6_flowinfo, yin6->sin6_flowinfo);
        CMP(xin6->sin6_scope_id, yin6->sin6_scope_id);
    }
#ifndef WIN32
	else if (x->sa_family == AF_UNIX)
	{
		struct sockaddr_un *xun = (void *)x, *yun = (void *)y;
		int                 r = strcmp(xun->sun_path, yun->sun_path);
		if (r != 0)
			return r;
	}
#endif
    else
    {
        assert(!"unknown sa_family");
    }

#undef CMP
    return 0;
}
