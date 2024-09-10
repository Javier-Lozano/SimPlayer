#include "../visual.h"
#include "../player.h"

void VisualWaves(SDL_Window *window, SDL_Renderer *renderer, Player *player, float *stream)
{
	SDL_Rect viewport;
	SDL_RenderGetViewport(renderer, &viewport);

	const float amplitude = viewport.h / 4;
	const int   samples   = player->spec.samples;
	const int   channels  = player->spec.channels;

	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderDrawLine(renderer, 0, viewport.h / 2, viewport.w, viewport.h / 2);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0xFF, 0xFF);
	SDL_RenderDrawLine(renderer, 0, amplitude, viewport.w, amplitude);
	SDL_RenderDrawLine(renderer, 0, amplitude * 3, viewport.w, amplitude * 3);

	SDL_SetRenderDrawColor(renderer, 0, 0xFF, 0, 0xFF);
	SDL_LockAudioDevice(player->device_id);
	for(int i = 0; i < (samples * channels) - 2; i++)
	{
		//SDL_FPoint p1;
		//SDL_FPoint p2;

		//p1.x = ((float)i * (float)viewport.w) / (float)samples;
		//p1.y = (stream[i]) ? -stream[i] * amplitude : 0;

		//p2.x = ((float)(i + 1) * (float)viewport.w) / (float)samples;
		//p2.y = (stream[i + 2]) ? -stream[i + 2] * amplitude : 0;

		printf("sample: %d ", i);
		if (i % channels == 0) // Left Channels
		{
			//SDL_RenderDrawLineF(renderer, p1.x, p1.y + amplitude, p2.x, p2.y + amplitude);
			printf("Left: %f ", stream[i]);
		}
		else // Right Channels
		{
			//SDL_RenderDrawLineF(renderer, p1.x, p1.y + (amplitude * 3), p2.x, p2.y + (amplitude * 3));
			printf("Right: %f\n", stream[i]);
		}
	}
	SDL_UnlockAudioDevice(player->device_id);
}
