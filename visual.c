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


void VisualMain(SDL_Window *window, SDL_Renderer *renderer, Player *player, Sint32 *stream)
{
	static float radians;
	const  float step = (V_2PI / 360.0f);

	int window_w;
	int window_h;
	SDL_GetWindowSize(window, &window_w, &window_h);
	const int border = 16;

//	static int previous_index;
//	static int current_index;
//
//	previous_index = current_index;
//	current_index = TrackListGetIndex(player->current);

	SDL_Rect demo_view = {
		.x = border,
		.y = border,
		.w = (3 * window_w / 4) - border,
	};
	SDL_Rect list_view = {
		.x = demo_view.x + demo_view.w + border,
		.y = border,
		.w = (window_w / 4) - (border * 2),
	};
	SDL_Rect progress_view = {
		.x = border,
		.y = window_h - 64 - border,
		.w = window_w - (border * 2),
		.h = 64
	};

	demo_view.h = window_h - progress_view.h - (border * 3);
	list_view.h = window_h - progress_view.h - (border * 3);

	radians += step;

	// Demo
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
	SDL_RenderSetViewport(renderer, &demo_view);
	SDL_RenderFillRect(renderer, NULL);
	VisualDemo(window, renderer, player, radians);
	if (player->current != NULL)
	{
		VisualWaves(window, renderer, player, stream);
	}

	// Track List
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
	SDL_RenderSetViewport(renderer, &list_view);
	SDL_RenderFillRect(renderer, NULL);

	// Progress Bar
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
	SDL_RenderSetViewport(renderer, &progress_view);
	SDL_RenderFillRect(renderer, NULL);

	SDL_SetRenderDrawColor(renderer, 0x80, 0x80, 0x80, 0xFF);
	SDL_RenderDrawLine(renderer, 0, progress_view.h / 2, progress_view.w, progress_view.h / 2);
	SDL_RenderDrawLine(renderer, progress_view.w / 2, 0, progress_view.w / 2, progress_view.h / 2);

//	if (current_index != previous_index && player->current != NULL)
//	{
//		// Update Name
//	}

	SDL_RenderSetViewport(renderer, NULL);
}
