#include "../visual.h"
#include "../player.h"

void VisualWaves(SDL_Window *window, SDL_Renderer *renderer, Player *player, float *stream)
{
	SDL_Rect viewport;
	SDL_RenderGetViewport(renderer, &viewport);

	const float amplitude = viewport.h / 4;
	const int   samples   = player->spec.samples;
	const int   channels  = player->spec.channels;
	const int   length    = samples * channels;

	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderDrawLine(renderer, 0, viewport.h / 2, viewport.w, viewport.h / 2);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0xFF, 0xFF);
	SDL_RenderDrawLine(renderer, 0, amplitude, viewport.w, amplitude);
	SDL_RenderDrawLine(renderer, 0, amplitude * 3, viewport.w, amplitude * 3);

	SDL_SetRenderDrawColor(renderer, 0, 0xFF, 0, 0xFF);
	SDL_LockAudioDevice(player->device_id);
	for(int i = 0; i < samples - 2; i ++)
	{
		int l_index = (i * channels);
		int r_index = l_index + 1;

		float x1 = (float)i       * (float)viewport.w / (float)samples;
		float x2 = (float)(i + 1) * (float)viewport.w / (float)samples;

		float l_y1 = (-stream[l_index]     * amplitude) + amplitude;
		float l_y2 = (-stream[l_index + 2] * amplitude) + amplitude;

		float r_y1 = (-stream[r_index]     * amplitude) + (amplitude * 3);
		float r_y2 = (-stream[r_index + 2] * amplitude) + (amplitude * 3);

		SDL_RenderDrawLineF(renderer, x1, l_y1, x2, l_y2);
		SDL_RenderDrawLineF(renderer, x1, r_y1, x2, r_y2);
	}
	SDL_UnlockAudioDevice(player->device_id);
}

