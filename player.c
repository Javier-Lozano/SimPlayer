#include "player.h"

static SDL_AudioDeviceID g_AudioDevice;
static Uint8 *g_StreamCopy;

static void callback(void *userdata, Uint8* stream, int len)
{
	Player *player = (Player*)userdata;
	Track  *track  = player->current->track;
	int    step    = 0;

	// Feed the right ammount of bytes to the audio stream
	step = (player->position > track->length - len) ? (track->length - player->position) : len;
	SDL_memcpy(stream, track->buffer + player->position, step);
	player->position += step;

	// Pause after finishing
	if (player->position >= track->length)
	{
		SDL_PauseAudioDevice(g_AudioDevice, 1);
		player->state = STATE_END_OF_TRACK;
	}

	// Copy stream
	if (g_StreamCopy != NULL)
	{
		SDL_memcpy(g_StreamCopy, stream, len);
	}
}

Uint8 normalize_stream(float *dest, SDL_AudioSpec *spec)
{
	const int channels = spec->channels;
	const int bytes    = SDL_AUDIO_BITSIZE(spec->format) / 8;
	const int length   = bytes * channels * SAMPLES;

	float sample_f32   = 0.0f;
	int   channel_i    = 0;
	int   sample_i     = 0;

	for(int i = 0; i < length; i += bytes)
	{
		channel_i = (i / bytes) % channels;
		sample_i  = (i / bytes) / channels;

		// SDL2 Audio only supports 32-bit floats
		if (SDL_AUDIO_ISFLOAT(spec->format))
		{
			SDL_memcpy(&sample_f32, g_StreamCopy + i, bytes);
		}
		else
		{
			switch(bytes)
			{
				case 1: // Uint8
					{
						Uint8 sample = 0;

						sample = g_StreamCopy[i];
						sample_f32 = ((float)sample * (2.0f / (float)UINT8_MAX)) - 1;
					}
					break;
				case 2: // SInt16
					{
						Sint16 sample = 0;

						SDL_memcpy(&sample, g_StreamCopy + i, bytes);
						sample_f32 = (float)sample / (float)(UINT16_MAX / 2);
					}
					break;
				case 4: // Sint32 (24-bit samples are treated as 32-bit samples)
					{
						Sint32 sample = 0;

						SDL_memcpy(&sample, g_StreamCopy + i, bytes);
						sample_f32 = (float)sample / (float)(UINT32_MAX / 2);
					}
					break;
			}
		}
		dest[(channel_i * SAMPLES) + sample_i] = sample_f32;
	}

	return 1;
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
				TrackNode *node = TracklistPush(event.drop.file, &player->tracklist);
				printf("Node %p\n", node);
				if (node != NULL && (player->state & STATE_STOP) == STATE_STOP)
				{
					player->current = node;
					player->state = STATE_PLAY;
					printf("Playing New\n");
				}
				break;
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym)
				{
					case SDLK_RETURN:
					case SDLK_SPACE:
						if (player->state == STATE_PAUSE || player->state == STATE_STOP) { player->state = STATE_PLAY; }
						else { player->state = STATE_PAUSE; }
						break;
					case SDLK_BACKSPACE:
						player->state = STATE_STOP;
						break;
				}
				break;
		}
	}

	// Player Events
	switch(player->state)
	{
		case STATE_STOP:
			if (SDL_GetAudioDeviceStatus(g_AudioDevice) != SDL_AUDIO_STOPPED)
			{
				SDL_CloseAudioDevice(g_AudioDevice);
			}
			break;
		case STATE_PLAY:
			if (player->current == NULL)
			{
				player->state = STATE_STOP;
			}
			else
			{
				if (SDL_GetAudioDeviceStatus(g_AudioDevice) == SDL_AUDIO_STOPPED)
				{
					PlayerPrepare(player);
				}
				else if (SDL_GetAudioDeviceStatus(g_AudioDevice) == SDL_AUDIO_PAUSED)
				{
					printf("Now is Playing\n");
					SDL_PauseAudioDevice(g_AudioDevice, 0);
				}
			}
			break;
		case STATE_PAUSE:
			if (SDL_GetAudioDeviceStatus(g_AudioDevice) == SDL_AUDIO_PLAYING)
			{
				const int bytes = SDL_AUDIO_BITSIZE(player->current->track->spec.format) / 8;
				const int channels = player->current->track->spec.channels;
				const int freq = player->current->track->spec.freq;
				const int length = player->current->track->length;
				float position = (float)player->position / (float)bytes / (float)channels / (float)freq;
				float duration = (float)length / (float)freq / (float)bytes / (float) channels;
				printf("Now is Paused\n");
				SDL_PauseAudioDevice(g_AudioDevice, 1);

				printf(
						"Position: %d\n"
						"Time: %f\n"
						"Duration: %f\n",
						player->position,
						position,
						duration
						);

			}
			break;
		case STATE_END_OF_TRACK:
			if (player->current->next == NULL)
			{
				player->current = player->tracklist.first;
				player->state = STATE_STOP;
			}
			else
			{
				player->current = player->current->next;
				player->state = STATE_PLAY;
				PlayerPrepare(player);
			}
			break;
	}

	if (player->is_running == 0)
	{
		SDL_CloseAudioDevice(g_AudioDevice);
	}
}

void PlayerDraw(Player *player, SDL_Renderer *renderer)
{
	static float samples[SAMPLES * 8]; // SDL suports 8 channels

	if (SDL_GetAudioDeviceStatus(g_AudioDevice) != SDL_AUDIO_STOPPED)
	{
		SDL_Window    *window  = SDL_RenderGetWindow(renderer);
		SDL_AudioSpec *spec    = &player->current->track->spec;
		const int     channels = spec->channels;
		int window_w  = 0;
		int window_h  = 0;

		// Get Window Size
		SDL_GetWindowSize(window, &window_w, &window_h);

		// Normalize Audio Stream
		if (normalize_stream(samples, spec) == 0)
		{
			fprintf(stderr, "Error: An error ocurred during normalization.\n");
			return;
		}

		// Draw Lines
		float ratio = (float)SAMPLES / (float)window_w;
		float split = (float)window_h / (float)channels / 2;
		float y1 = 0;
		float y2 = 0;

		// This draws 1 or 2 channels for now
		for(int i = 0; i < channels; i++)
		{
			if( i > 0)
			{
				SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderDrawLineF(renderer, 0, (split * i * 2), window_w, (split * i * 2));
			}
			SDL_SetRenderDrawColor(renderer, 0x80, 0x80, 0xFF, 0xFF);
			SDL_RenderDrawLineF(renderer, 0, (split * i * 2) + split, window_w, (split * i * 2) + split);
		}

		// TODO: Find a better way to draw lines
		SDL_SetRenderDrawColor(renderer, 0, 0xFF, 0, 0xFF);
		for(int i = 0; i < window_w - 1; i++)
		{
			for(int c = 0; c < channels; c++)
			{
				int j = (ratio * i) + (SAMPLES * c);
				int k = (ratio * (i + 1)) + (SAMPLES * c);
				// Reverse sample to show it properly | +1 is Up | -1 is Down |
				y1 = -samples[j] * split;
				y2 = -samples[k] * split;
				SDL_RenderDrawLineF(renderer, i, y1 + (split * c * 2) + split, i+1, y2 + (split * c * 2) + split);
			}
		}
	}
}

void PlayerPrepare(Player *player)
{
	SDL_AudioSpec spec;
	size_t stream_copy_bytes;

	// Check for a valid node
	if (player->current == NULL) { return; }

	// Close previous audio device
	if (SDL_GetAudioDeviceStatus(g_AudioDevice) != SDL_AUDIO_STOPPED)
	{
		SDL_CloseAudioDevice(g_AudioDevice);
	}

	// Check stream copy
	if (g_StreamCopy != NULL) { SDL_free(g_StreamCopy); }

	// Setup Callback and samples to feed in
	spec = player->current->track->spec;
	spec.samples = SAMPLES;
	spec.callback = callback;
	spec.userdata = (void*) player;

	printf("Bytes: %d\n", SDL_AUDIO_BITSIZE(spec.format) / 8);

	// Open Audio Device
	g_AudioDevice = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, 0);
	if (SDL_GetAudioDeviceStatus(g_AudioDevice) == SDL_AUDIO_STOPPED)
	{
		printf("Error: Player can't play the current track.\n");
		if (player->current->next == NULL)
		{
			printf("Player STOPPED!\n");
		}
		else
		{
			printf("Skipping to next track.\n");
			player->current = player->current->next;
			PlayerPrepare(player);
		}
		return;
	}

	// Allocate space for stream copy
	stream_copy_bytes = (SDL_AUDIO_BITSIZE(spec.format) / 8) * spec.channels * SAMPLES;
	g_StreamCopy = SDL_malloc(stream_copy_bytes);
	SDL_memset(g_StreamCopy, 0, stream_copy_bytes);

	// Reset player position and unpause Audio Device
	player->position = 0;
	SDL_PauseAudioDevice(g_AudioDevice, 0);

}

