#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <stdbool.h>
#include "SDL.h"
#include "track_list.h"

typedef struct player_st {
	bool          is_running;
	bool          is_paused;
	Uint32        position;
	TrackList     track_list;
	TrackNode    *current;
	SDL_AudioSpec spec;
} Player;

bool PlayerInit(Player *player, Uint8 channels, int sampling_rate, int samples);
void PlayerClose(Player *player);

void PlayerUpdate(Player *player);
void PlayerDraw(Player *player, SDL_Renderer *renderer);

void PlayerPlay(Player *player);
void PlayerStop(Player *player);

#endif // _PLAYER_H_
