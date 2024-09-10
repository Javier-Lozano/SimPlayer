#include "../visual.h"
#include "../player.h"

#define TRIANGLE_COUNT (50)

typedef struct triangle_st {
	SDL_FPoint a, b, c;
} Triangle;

void VisualDemo(SDL_Window *window, SDL_Renderer *renderer, Player *player, float radians)
{
	static bool do_once= false;
	static bool mouse_effect = false;

	static SDL_Texture *texture;
	static SDL_Rect     rect;

	static Triangle triangles[TRIANGLE_COUNT];

	SDL_Rect viewport;
	SDL_RenderGetViewport(renderer, &viewport);
	SDL_FPoint center;

	float radius = viewport.w / 4;

	// Do once
	if (!do_once)
	{
		do_once = false;

		TTF_SetFontSize(g_Font, 64);
		SDL_Surface *surface = TTF_RenderText_Solid(g_Font, "Drop a WAV file", (SDL_Color){255,255,255,255});
		TTF_SetFontSize(g_Font, 16);

		texture = SDL_CreateTextureFromSurface(renderer, surface);
		rect.x = (viewport.w / 2) - (surface->w / 2);
		rect.y = (viewport.h / 2) - (surface->h / 2);
		rect.w = surface->w;
		rect.h = surface->h;

		SDL_FreeSurface(surface);
	}

	// Update

	// Find center
	center.x = viewport.w / 2;
	center.y = viewport.h / 2;

	if (mouse_effect)
	{
	}

	triangles[0].a = (SDL_FPoint) {
		(SDL_cos(radians) * radius) + (viewport.w / 2),
		(SDL_sin(radians) * radius) + (viewport.h / 2)
	};
	triangles[0].b = (SDL_FPoint) {
		.x = (SDL_cos(radians + (V_2PI / 3)) * radius) + (viewport.w / 2),
		.y = (SDL_sin(radians + (V_2PI / 3)) * radius) + (viewport.h / 2)
	};
	triangles[0].c = (SDL_FPoint) {
		.x = (SDL_cos(radians - (V_2PI / 3)) * radius) + (viewport.w / 2),
		.y = (SDL_sin(radians - (V_2PI / 3)) * radius) + (viewport.h / 2)
	};

	for(int i = TRIANGLE_COUNT - 1; i >= 0; i--)
	{
		if (i > 0)
		{
			triangles[i] = triangles[i - 1];
		}
		float alpha = (TRIANGLE_COUNT - (float)i) * 255.0f / 100.0f;
		SDL_SetRenderDrawColor(renderer, 0x20, 0x40, 0xFF, alpha);

		SDL_RenderDrawLineF(renderer, triangles[i].a.x, triangles[i].a.y, triangles[i].b.x, triangles[i].b.y);
		SDL_RenderDrawLineF(renderer, triangles[i].b.x, triangles[i].b.y, triangles[i].c.x, triangles[i].c.y);
		SDL_RenderDrawLineF(renderer, triangles[i].c.x, triangles[i].c.y, triangles[i].a.x, triangles[i].a.y);
	}

	if (player->track_list.track_count == 0)
	{
		// Draw Text
		SDL_RenderCopy(renderer, texture, NULL, &rect);
	}
	else
	{
		// Draw Black Box 50% opacity
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0x80);
		SDL_RenderFillRect(renderer, NULL);
	}

	// Cleanup
	if (!player->is_running)
	{
		SDL_DestroyTexture(texture);
	}
}
