#include "visual.h"
#include "player.h"

TTF_Font *g_Font;

bool VisualInit(SDL_Renderer *renderer)
{
	g_Font = TTF_OpenFont("assets/ModernDOS8x8.ttf", 16);
	if (g_Font == NULL)
	{
		fprintf(stderr, "Error: Couldn't open font.. %s.\n", TTF_GetError());
		return false;
	}

	return true;
}

void VisualClose()
{
	TTF_CloseFont(g_Font);
}


void VisualMain(SDL_Window *window, SDL_Renderer *renderer, Player *player, float *stream)
{
	static float radians;
	const  float step = (V_2PI / 360.0f);
	int window_w;
	int window_h;
	SDL_GetWindowSize(window, &window_w, &window_h);

	SDL_Rect demo_viewport = {
		BORDER,
		BORDER,
		window_w - (BORDER * 2),
		window_h - (BORDER * 2)
	};

	radians += step;

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
	SDL_RenderSetViewport(renderer, &demo_viewport);
	SDL_RenderFillRect(renderer, NULL);

	VisualDemo(window, renderer, player, radians);
	if (player->current != NULL)
	{
		VisualWaves(window, renderer, player, stream);
	}
	SDL_RenderSetViewport(renderer, NULL);
}
