#include "player.h"

static SDL_AudioDeviceID  g_AudioDeviceID;
static float *g_StreamCopy;

static void callback(void *userdata, Uint8* stream, int len);
//static Uint8 LoadTrack(const char *path, Player *player);

bool PlayerInit(Player *player, Uint8 channels, int sampling_rate, int samples)
{
	SDL_AudioSpec spec = {
		.freq     = sampling_rate,
		.format   = AUDIO_F32,
		.channels = channels,
		.callback = callback,
		.samples  = samples,
		.userdata = (void*)player
	};

	// Open Audio Device
	g_AudioDeviceID = SDL_OpenAudioDevice(NULL, 0, &spec, &player->spec, 0);
	if (g_AudioDeviceID == 0)
	{
		fprintf(stderr, "Error: Couldn't open audio device. %s.\n", SDL_GetError());
		return false;
	}

	// Unpause Audio Device
	SDL_PauseAudioDevice(g_AudioDeviceID, 0);

	if (!TrackListInit(&player->track_list))
	{
		fprintf(stderr, "Error: Track list initialization failed!.\n");
		return true;
	}

	// Allocate Stream Copy
	g_StreamCopy = (float*)SDL_malloc(sizeof(float) * samples * channels);

	player->is_running = true;
	player->is_paused  = false;
	player->position   = 0;

	return true;
}

void PlayerClose(Player *player)
{
	SDL_CloseAudioDevice(g_AudioDeviceID);
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
						SDL_PauseAudioDevice(g_AudioDeviceID, player->is_paused);
						break;
					case SDLK_BACKSPACE:
				}
				break;
		}
	}

}

void PlayerDraw(Player *player, SDL_Renderer *renderer)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
	SDL_RenderClear(renderer);

	// Draw stuff

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
	Track *track = player->current->track;
	int step = (player->position + len > track->length) ? track->length - player->position : len;

	if (step <= 0)
	{
		player->current = player->current->next;
	}
	else
	{
		// Output stream
		SDL_memcpy(stream, track->buffer + player->position, step);
		// Copy Stream
		SDL_memcpy(g_StreamCopy, stream, len);

		player->position += step;
	}
}

