#include "track_list.h"

static bool LoadTrack(const char *path, Track *track, SDL_AudioSpec *target_spec);
static void PrintTrackList(TrackList *track_list);

bool TrackListInit(TrackList *track_list)
{
	Track     *tracks = NULL;
	TrackNode *nodes  = NULL;

	tracks = SDL_malloc(sizeof(Track) * TRACK_LIST_CHUNK);
	nodes  = SDL_malloc(sizeof(TrackNode) * TRACK_LIST_CHUNK);
	if (tracks == NULL || nodes == NULL)
	{
		fprintf(stderr, "Error: It seems you ran out of memory! =(\n");
		return false;
	}

	SDL_memset(tracks, 0, sizeof(Track) * TRACK_LIST_CHUNK);
	SDL_memset(nodes, 0, sizeof(TrackNode) * TRACK_LIST_CHUNK);

	track_list->tracks = tracks;
	track_list->nodes  = nodes;
	//track_list->allocations = 1;
	track_list->track_count = 0;

	return true;
}

void TrackListClose(TrackList *track_list)
{
	const int length = TRACK_LIST_CHUNK /* * track_list->allocations */;
	Track *tracks    = track_list->tracks;

	// Free buffers
	for(int i = 0; i < length; i++)
	{
		if (tracks[i].buffer != NULL)
		{
			SDL_free(tracks[i].buffer);
		}
	}

	// Free arrays
	SDL_free(track_list->nodes);
	SDL_free(track_list->tracks);
	//SDL_memset(track_list, 0, sizeof(TrackList));
}

bool TrackListInsert(const char *path, TrackList *track_list, SDL_AudioSpec *target_spec)
{
	int       track_i = -1;
	int       node_i  = -1;
	Track     *track  = track_list->tracks;
	TrackNode *node   = track_list->nodes;

	// Find uninitialized tracks and nodes
	for(int i = 0; i < (TRACK_LIST_CHUNK /* * track_list->allocations*/); i++)
	{
		if (track_i == -1 && track[i].buffer == NULL) { track_i = i; } // Found an empty track slot
		if (node_i  == -1 && node[i].track   == NULL) { node_i  = i; } // Found an empty node slot
		if (track_i >= 0 && node_i >= 0) { break; } // Break loop when both are found
	}

	// TODO: Resize arrays if track_list is full
	if (track_i == -1 || node_i == -1)
	{
		fprintf(stderr, "Error: It seems track list is full!\nTODO: Make track list dynamic\n");
		return false;
	}

	// Adjust Track and Node 
	track = track + track_i; // Is the same as doing: track = &track[track_i];
	node  = node  + node_i;  //                       node  = &node[node_i];

	if (!LoadTrack(path, track, target_spec))
	{
		fprintf(stderr, "Error: Couldn't load track.\n");
		return false;
	}

	// Initialize Linked List
	if (track_list->track_count == 0)
	{
		track_list->first = node;
		track_list->last  = node;
	}
	else
	{
		track_list->last->next = node;
		node->previous   = track_list->last;
		track_list->last = node;
	}

	node->track = track;
	track_list->track_count++;

	PrintTrackList(track_list);

	return true;
}

static bool LoadTrack(const char *path, Track *track, SDL_AudioSpec *target_spec)
{
	const int bytes     = SDL_AUDIO_BITSIZE(target_spec->format) / 8;
	const int channels  = target_spec->channels;
	const int frequency = target_spec->freq;

	SDL_AudioStream *audio_stream;
	int              stream_length;

	Uint8           *wav_buffer;
	Uint32           wav_length;
	SDL_AudioSpec    wav_spec;

	// Load Wav
	if (SDL_LoadWAV(path, &wav_spec, &wav_buffer, &wav_length) == NULL)
	{
		fprintf(stderr, "Error: Couldn't load WAV. %s\n", SDL_GetError());
		return 0;
	}
	printf("Length Check 1:%d\n", wav_length);

	audio_stream = SDL_NewAudioStream(wav_spec.format, wav_spec.channels, wav_spec.freq, target_spec->format, channels, frequency);
	if (audio_stream == NULL)
	{
		fprintf(stderr, "Couldn't create audio stream. %s.\n", SDL_GetError());
		goto load_track_failure;
	}
	if (SDL_AudioStreamPut(audio_stream, wav_buffer, wav_length) == -1)
	{
		fprintf(stderr, "Couldn't insert data into audio stream. %s.\n", SDL_GetError());
		goto load_track_failure;
	}
	if (SDL_AudioStreamFlush(audio_stream) == -1)
	{
		fprintf(stderr, "Couldn't flush audio stream. %s.\n", SDL_GetError());
		goto load_track_failure;
	}

	stream_length = SDL_AudioStreamAvailable(audio_stream);
	printf("Length Check 2:%d\n", stream_length);

	track->buffer = (Uint8*)SDL_malloc(stream_length);
	if (track->buffer == NULL)
	{
		fprintf(stderr, "Error: It seems you ran out of memory! =(\n");
		goto load_track_failure;
	}
	track->length = stream_length;

	// Get Converted audio
	int count = SDL_AudioStreamGet(audio_stream, track->buffer, stream_length);
	printf("Length Check 3:%d\n", count);
	if (count == -1)
	{
		fprintf(stderr, "Couldn't get data from audio stream. %s.\n", SDL_GetError());
		goto load_track_failure;
	}

	SDL_FreeAudioStream(audio_stream);
	SDL_FreeWAV(wav_buffer);

	printf("LoadTrack success! \n");
	return true;

load_track_failure:
	SDL_FreeAudioStream(audio_stream);
	SDL_FreeWAV(wav_buffer);
	printf("LoadTrack Failed\n");
	return false;
}

static void PrintTrackList(TrackList *track_list)
{
	TrackNode *node = track_list->first;

	if (node == NULL) { printf("Empty!\n"); }
	else { printf("Track Count: %d\n", track_list->track_count); }

	for (int i = 1; node != NULL; i++)
	{
		printf("Index: %d\n", i);
		printf("\tNext:     %p\n", node->next);
		printf("\tPrevious: %p\n", node->previous);
		printf("\tTrack:    %p\n", node->track);
		node = node->next;
	}
}

