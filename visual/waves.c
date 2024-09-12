#include <limits.h>
#include "../visual.h"
#include "../player.h"

void VisualWaves(SDL_Window *window, SDL_Renderer *renderer, Player *player, Sint32 *stream)
{
	SDL_Rect viewport;
	SDL_RenderGetViewport(renderer, &viewport);

	const int   samples   = player->spec.samples;
	const int   channels  = player->spec.channels;
	const int   length    = samples * channels;
	float amplitude = viewport.h / 2 / channels;

	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	for(int i = 1; i < channels; i++)
	{
		SDL_RenderDrawLine(renderer, 0, viewport.h / (i * channels), viewport.w, viewport.h / (i * channels));
	}

	SDL_SetRenderDrawColor(renderer, 0, 0, 0xFF, 0xFF);
	for(int i = 1; i < channels * 2; i++)
	{
		if (i % 2 == 1)
		{
			SDL_RenderDrawLine(renderer, 0, amplitude * i, viewport.w, amplitude * i);
		}
	}

	SDL_SetRenderDrawColor(renderer, 0, 0xFF, 0, 0xFF);
	SDL_LockAudioDevice(player->device_id);
	for(int i = 0; i < samples - 2; i ++)
	{
		int l_index = (i * channels);
		int r_index = l_index + 1;

		float x1 = (float)i       * (float)viewport.w / (float)samples;
		float x2 = (float)(i + 1) * (float)viewport.w / (float)samples;

		double l_v1 = (double)stream[l_index] / (double) INT_MAX;
		double l_v2 = (double)stream[l_index + 2] / (double) INT_MAX;

		double l_y1 = -(l_v1 * amplitude) + amplitude;
		double l_y2 = -(l_v2 * amplitude) + amplitude;

		double r_v1 = (double)stream[r_index] / (double) INT_MAX;
		double r_v2 = (double)stream[r_index + 2] / (double) INT_MAX;

		double r_y1 = -(r_v1 * amplitude) + (amplitude * 3);
		double r_y2 = -(r_v2 * amplitude) + (amplitude * 3);

		//printf("%f %f %d\n",l_y1, l_y2, INT_MAX);
		//printf("%f %f %d\n",r_y1, r_y2, INT_MAX);

		SDL_RenderDrawLineF(renderer, x1, l_y1, x2, l_y2);
		SDL_RenderDrawLineF(renderer, x1, r_y1, x2, r_y2);
	}
	SDL_UnlockAudioDevice(player->device_id);
}

