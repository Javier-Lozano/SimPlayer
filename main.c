#include "SDL.h"
#include "player.h"

#define WINDOW_WIDTH  (800)
#define WINDOW_HEIGHT (600)

static Uint8 InitSDL(SDL_Window **window, SDL_Renderer **renderer);
static void CloseSDL(SDL_Window **window, SDL_Renderer **renderer);

int main( int argc, char **argv)
{
	SDL_Window   *window   = NULL;
	SDL_Renderer *renderer = NULL;
	Player       player    = {0};

	// TODO: Load files from command line

	if (InitSDL(&window, &renderer) == 0 || InitTracklist(&player.tracklist) == 0)
	{
		return 1;
	}
	player.is_running = 1;
	player.state = STATE_STOP;

	while(player.is_running)
	{
		// Update
		PlayerUpdate(&player);
		// Clear Screen
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
		SDL_RenderClear(renderer);
		// Draw Player
		PlayerDraw(&player, renderer);
		// Present to screen
		SDL_RenderPresent(renderer);
	}

	CloseTracklist(&player.tracklist);
	CloseSDL(&window, &renderer);

	printf("\nSEE YOU SPACE COWBOY\n");

	return 0;
}

static Uint8 InitSDL(SDL_Window **window, SDL_Renderer **renderer)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		fprintf(stderr, "Error: Couldn't init SDL. %s\n", SDL_GetError());
		return 0;
	}
	*window = SDL_CreateWindow("Sim(ple) Music Player SDL", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (*window == NULL)
	{
		fprintf(stderr, "Error: Couldn't create window. %s\n", SDL_GetError());
		return 0;
	}
	*renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (*renderer == NULL)
	{
		fprintf(stderr, "Error: Couldn't create renderer. %s\n", SDL_GetError());
		return 0;
	}
	return 1;
}

static void CloseSDL(SDL_Window **window, SDL_Renderer **renderer)
{
	SDL_DestroyRenderer(*renderer);
	SDL_DestroyWindow(*window);
	SDL_Quit();
}
