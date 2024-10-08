#ifndef _TRACK_LIST_H_
#define _TRACK_LIST_H_

#include "SDL.h"
#include <stdbool.h>

#define TRACK_LIST_CHUNK (256)

typedef struct track_st {
	SDL_AudioStream *stream;
	SDL_AudioSpec    spec;
	Uint32           length; // In samples
	Uint8           *buffer; 
} Track;

typedef struct track_node_st { // Linked List nodes
	struct track_node_st *previous;
	struct track_node_st *next;
	struct track_st      *track;
} TrackNode;

typedef struct track_list_st {
	int        track_count;
	//int        allocations;
	Track     *tracks;
	TrackNode *nodes;
	TrackNode *first;
	TrackNode *last;
} TrackList;

bool TrackListInit(TrackList *track_list);
void TrackListClose(TrackList *track_list);
bool TrackListInsert(const char *path, TrackList *track_list, SDL_AudioSpec *target_spec);

int TrackProcessAudio(Track *track, int position, SDL_AudioSpec *player_spec, Uint8 *output);

#endif // _TRACK_LIST_H_

