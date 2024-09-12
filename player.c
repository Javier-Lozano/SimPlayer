#include "player.h"
#include "SDL_ttf.h"

static Uint8 *g_StreamCopy;

static void callback(void *userdata, Uint8* stream, int len);

bool PlayerInit(SDL_Renderer *renderer, Player *player, Uint8 channels, int sampling_rate, int samples)
{
	SDL_AudioSpec spec = {
		.freq     = sampling_rate,
		.format   = AUDIO_S32,
		.channels = channels,
		.callback = callback,
		.samples  = samples,
		.userdata = (void*)player
	};

	// Open Audio Device
	player->device_id = SDL_OpenAudioDevice(NULL, 0, &spec, &player->spec, 0);
	if (player->device_id == 0)
	{
		fprintf(stderr, "Error: Couldn't open audio device. %s.\n", SDL_GetError());
		return false;
	}

	// Unpause Audio Device
	SDL_PauseAudioDevice(player->device_id, 0);

	if (!TrackListInit(&player->track_list))
	{
		fprintf(stderr, "Error: Track list initialization failed!.\n");
		return true;
	}

	// Allocate Stream Copy
	g_StreamCopy = (Uint8*)SDL_malloc(sizeof(Sint32) * samples * channels);

	// Initialize Visual
	if (!VisualInit(renderer))
	{
		return false;
	}

	player->is_running = true;
	player->is_paused  = false;
	player->position   = 0;

	return true;
}

void PlayerClose(Player *player)
{
	VisualClose();
	SDL_CloseAudioDevice(player->device_id);
	TrackListClose(&player->track_list);
	SDL_free(g_StreamCopy);
}

void PlayerUpdate(Player *player)
{
	// SDL Events
	SDL_Event event;

	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_QUIT:
				player->is_running = 0;
				break;
			case SDL_DROPFILE:
				{
					char *file_path = event.drop.file;
					if (TrackListInsert(file_path, &player->track_list, &player->spec) && player->current == NULL)
					{
						player->current = player->track_list.last;
					}
					SDL_free(file_path);
				}
				break;
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym)
				{
					case SDLK_RETURN:
					case SDLK_SPACE:
						player->is_paused = !player->is_paused;
						SDL_PauseAudioDevice(player->device_id, player->is_paused);
						break;
					case SDLK_BACKSPACE:
						break;
				}
				break;
			case SDL_WINDOWEVENT:
				break;
		}
	}

}


void PlayerDraw(SDL_Renderer *renderer, SDL_Window *window, Player *player)
{
	// Clear Screen
	SDL_SetRenderDrawColor(renderer, 0x40, 0, 0, 0xFF);
	SDL_RenderClear(renderer);

	VisualMain(window, renderer, player, (Sint32*)g_StreamCopy);

	// Present
	SDL_RenderPresent(renderer);
}

static void callback(void *userdata, Uint8* stream, int len)
{
	Player *player = (Player*)userdata;

	// Play silence
	if (player->current == NULL)
	{
		SDL_memset(stream, 0, len);
		return;
	}

	// Play Track
	int step = TrackProcessAudio(player->current->track, player->position, &player->spec, stream);
	player->position += step;

	if (step == 0)
	{
		player->current = player->current->next;
		player->position = 0;
	}

	// Copy
	SDL_memcpy(g_StreamCopy, stream, len);
}

