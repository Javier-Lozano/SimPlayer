#ifndef _TRACKLIST_H_
#define _TRACKLIST_H_

#include "SDL.h"

#define TRACKLIST_MAX (256)

typedef struct track_st {
	Uint8         *buffer;
	Uint32        length;
	SDL_AudioSpec spec;
} Track;

typedef struct track_node_st {
	struct track_node_st *previous;
	struct track_node_st *next;
	struct track_st      *track;
} TrackNode;

typedef struct tracklist_st {
	int       track_count;
	int       allocations;
	Track     *tracks;
	TrackNode *nodes;
	TrackNode *first;
	TrackNode *last;
} Tracklist;

Uint8 InitTracklist(Tracklist *tracklist /*, CMDInfo *info*/);
void CloseTracklist(Tracklist *tracklist);
TrackNode *TracklistPush(const char *path, Tracklist *tracklist);

#endif // _TRACKLIST_H_

