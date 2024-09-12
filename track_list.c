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

static bool LoadTrack(const char *path, Track *track, SDL_AudioSpec *dst_spec)
{
	SDL_AudioStream *stream;
	Uint8           *buffer;
	Uint32           length;
	SDL_AudioSpec    spec;

	// Load Wav
	if (SDL_LoadWAV(path, &spec, &buffer, &length) == NULL)
	{
		fprintf(stderr, "Error: Couldn't load WAV. %s\n", SDL_GetError());
		return false;
	}

	stream = SDL_NewAudioStream(spec.format, spec.channels, spec.freq, dst_spec->format, dst_spec->channels, dst_spec->freq);
	if (stream == NULL)
	{
		fprintf(stderr, "Couldn't create audio stream. %s.\n", SDL_GetError());
		SDL_FreeWAV(buffer);
		return false;
	}

	track->stream = stream;
	track->buffer = buffer;
	track->length = length;
	track->spec   = spec;

	return true;
}

int TrackProcessAudio(Track *track, int position, SDL_AudioSpec *player_spec, Uint8 *output)
{
	const int samples  = player_spec->samples;
	const int channels = track->spec.channels;
	const int bytes    = SDL_AUDIO_BITSIZE(track->spec.format) / 8;
	const int chunk    = samples * channels * bytes;

	int step = (position + chunk > track->length) ? track->length - position : chunk;
	//printf("Step %d (bytes: %d samples: %d channels:%d)\n", step, bytes, samples, channels);

	// Get
	int available = SDL_AudioStreamAvailable(track->stream);
	//printf("AudioStream Available: %d\n", available);
	SDL_AudioStreamGet(track->stream, output, available);

	if (step > 0)
	{
		// Put
		SDL_AudioStreamPut(track->stream, track->buffer + position, step);
		SDL_AudioStreamFlush(track->stream);
	}

	return step;
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

