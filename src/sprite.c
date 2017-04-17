#include "sprite.h"

void _sprite_apply_align(sprite_t * spr);
void _sprite_apply_color(sprite_t * spr);

void sprite_init(sprite_t * spr)
{
	spr->_texture = NULL;
	spr->_src = (rect_t) { 0, 0, 0, 0 };
	spr->_dst = (rect_t) { 0, 0, 0, 0 };
	spr->_pos = (vec2f_t) { 0.0f, 0.0f };
	spr->_color = (color_t) { 255, 255, 255, 255 };
	spr->_align = ALIGN_TOP_LEFT;
}

bool sprite_load_file(sprite_t * spr, const char * filename)
{
	int w, h;

	spr->_pos = (vec2f_t) { 0.0f, 0.0f };
	spr->_color = (color_t) { 255, 255, 255, 255 };
	spr->_align = ALIGN_TOP_LEFT;

	spr->_texture = image_load(filename, g_renderer);
    if (NULL == spr->_texture)
    {
        return false;
    }

	SDL_QueryTexture(spr->_texture, NULL, NULL, &w, &h);
	spr->_src = (rect_t) { 0, 0, w, h };
	spr->_dst = (rect_t) { 0, 0, w, h };

	_sprite_apply_align(spr);
	_sprite_apply_color(spr);

    return true;
}

bool sprite_create(sprite_t * spr, int w, int h, color_t color)
{
	uint8_t pixel[] = { 255, 255, 255, 255 };
	spr->_texture = SDL_CreateTexture(g_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, w, h);
	if (NULL == spr->_texture)
	{
		return false;
	}
	SDL_UpdateTexture(spr->_texture, NULL, &pixel, sizeof(pixel));

	spr->_pos = (vec2f_t) { 0.0f, 0.0f };
	spr->_color = (color_t) { 255, 255, 255, 255 };
	spr->_align = ALIGN_TOP_LEFT;

	spr->_src = (rect_t) { 0, 0, w, h };
	spr->_dst = (rect_t) { 0, 0, w, h };

	_sprite_apply_align(spr);
	_sprite_apply_color(spr);

	return true;
}

void sprite_cleanup(sprite_t * spr)
{	
	if (NULL != spr->_texture)
	{
		SDL_DestroyTexture(spr->_texture);
		spr->_texture = NULL;
	}
}

void sprite_render(sprite_t * spr)
{
    if (NULL != spr->_texture)
    {
        SDL_RenderCopy(g_renderer, spr->_texture, &spr->_src, &spr->_dst);
    }
}

void sprite_set_align(sprite_t * spr, align_t align)
{
	spr->_align = align;
	_sprite_apply_align(spr);
}

void sprite_set_pos(sprite_t * spr, vec2f_t pos)
{
	spr->_pos = pos;
	spr->_dst.x = (int)spr->_pos.x;
	spr->_dst.y = (int)spr->_pos.y;
	_sprite_apply_align(spr);
}

void sprite_set_size(sprite_t * spr, vec2_t size)
{
	spr->_dst.w = size.x;
	spr->_dst.h = size.y;
	_sprite_apply_align(spr);
}

void sprite_set_color(sprite_t * spr, color_t color)
{
	spr->_color = color;
	_sprite_apply_color(spr);
}

void _sprite_apply_align(sprite_t * spr)
{
	if (ALIGN_TOP_LEFT == spr->_align)
	{
		spr->_dst.x = (int)spr->_pos.x;
		spr->_dst.y = (int)spr->_pos.y;
	}
	else if (ALIGN_CENTER == spr->_align)
	{
		spr->_dst.x = (int)spr->_pos.x - (spr->_dst.w / 2);
		spr->_dst.y = (int)spr->_pos.y - (spr->_dst.h / 2);
	}
}

void _sprite_apply_color(sprite_t * spr)
{
	if (NULL != spr->_texture)
	{
		SDL_SetTextureColorMod(spr->_texture, spr->_color.r, spr->_color.g, spr->_color.b);
	}
}
