#include "tracklist.h"

Uint8 InitTracklist(Tracklist *tracklist /*, CMDInfo *info*/)
{
	Track     *tracks = NULL;
	TrackNode *nodes  = NULL;

	tracks = SDL_malloc(sizeof(Track) * TRACKLIST_MAX);
	nodes = SDL_malloc(sizeof(TrackNode) * TRACKLIST_MAX);
	if (tracks == NULL || nodes == NULL)
	{
		fprintf(stderr, "Error: It seems you ran out of memory!\n");
		return 0;
	}

	SDL_memset(tracks, 0, sizeof(Track) * TRACKLIST_MAX);
	SDL_memset(nodes, 0, sizeof(TrackNode) * TRACKLIST_MAX);

	tracklist->tracks = tracks;
	tracklist->nodes = nodes;
	tracklist->allocations = 1;
	tracklist->track_count = 0;

	return 1;
}

void CloseTracklist(Tracklist *tracklist)
{
	Track *tracks = tracklist->tracks;

	// Free buffers
	for(int i = 0; i < (TRACKLIST_MAX * tracklist->allocations); i++)
	{
		if (tracks[i].buffer != NULL)
		{
			SDL_FreeWAV(tracks[i].buffer);
		}
	}

	// Free arrays
	SDL_free(tracklist->nodes);
	SDL_free(tracklist->tracks);
	SDL_memset(tracklist, 0, sizeof(Tracklist));
}

TrackNode *TracklistPush(const char *path, Tracklist *tracklist)
{
	int       track_i = -1;
	int       node_i  = -1;
	Track     *tracks = tracklist->tracks;
	TrackNode *nodes  = tracklist->nodes;
	Track     *t      = NULL;
	TrackNode *n      = NULL; 

	// Find uninitialized tracks and nodes
	for(int i = 0; i < (TRACKLIST_MAX * tracklist->allocations); i++)
	{
		if (track_i == -1 && tracks[i].buffer == NULL) { track_i = i; }
		if (node_i == -1 && nodes[i].track == NULL) { node_i = i; }

		if (track_i >= 0 && node_i >= 0) { break; }
	}

	// TODO: Resize arrays if tracklist is full
	if (track_i < 0) 
	{
		return NULL;
	}

	t = tracks + track_i;
	n = nodes + node_i;

	// Load new Track
	if (SDL_LoadWAV(path, &t->spec, &t->buffer, &t->length) == NULL)
	{
		fprintf(stderr, "Error: Couldn't load WAV. %s\n", SDL_GetError());
		return NULL;
	}

	// Initialize Node
	n->track = t;

	if (tracklist->track_count == 0)
	{
		tracklist->first = n;
		tracklist->last = n;
	}
	else
	{
		tracklist->last->next = n;
		n->previous = tracklist->last;
		tracklist->last = n;
	}

	tracklist->track_count++;

	return n;
}

