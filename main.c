#include <stdbool.h>
#include "SDL.h"
#include "player.h"
#include "visual.h"

#define WINDOW_WIDTH  (800)
#define WINDOW_HEIGHT (600)

static bool SDLInit(SDL_Window **window, SDL_Renderer **renderer);
static void SDLClose(SDL_Window **window, SDL_Renderer **renderer);

int main( int argc, char **argv)
{
	SDL_Window   *window   = NULL;
	SDL_Renderer *renderer = NULL;
	Player        player   = {0};

	// TODO: Load files from command line

	if (!SDLInit(&window, &renderer) || !PlayerInit(renderer, &player, 2, 44100, 1024))
	{
		return 1;
	}

	while(player.is_running)
	{
		// Update
		PlayerUpdate(&player);
		// Draw
		PlayerDraw(renderer, window, &player);
	}

	PlayerClose(&player);
	SDLClose(&window, &renderer);

	printf("\nSEE YOU SPACE COWBOY\n");

	return 0;
}

static bool SDLInit(SDL_Window **window, SDL_Renderer **renderer)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		fprintf(stderr, "Error: Couldn't init SDL. %s\n", SDL_GetError());
		return false;
	}
	if (TTF_Init() == -1)
	{
		fprintf(stderr, "Error: Couldn't init SDL2_ttf. %s\n", TTF_GetError());
		return false;
	}
	*window = SDL_CreateWindow("Sim(ple) Music Player SDL", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (*window == NULL)
	{
		fprintf(stderr, "Error: Couldn't create window. %s\n", SDL_GetError());
		return false;
	}
	*renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (*renderer == NULL)
	{
		fprintf(stderr, "Error: Couldn't create renderer. %s\n", SDL_GetError());
		return false;
	}
	SDL_SetRenderDrawBlendMode(*renderer, SDL_BLENDMODE_BLEND);
	return true;
}

static void SDLClose(SDL_Window **window, SDL_Renderer **renderer)
{
	TTF_Quit();
	SDL_DestroyRenderer(*renderer);
	SDL_DestroyWindow(*window);
	SDL_Quit();
}
