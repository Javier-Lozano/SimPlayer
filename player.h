#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <stdbool.h>
#include "SDL.h"
#include "track_list.h"
#include "visual.h"

typedef struct player_st {
	bool              is_running;
	bool              is_paused;
	Uint32            position;
	TrackList         track_list;
	TrackNode        *current;
	SDL_AudioSpec     spec;
	SDL_AudioDeviceID device_id;
} Player;

bool PlayerInit(SDL_Renderer *renderer, Player *player, Uint8 channels, int sampling_rate, int samples);
void PlayerClose(Player *player);

void PlayerUpdate(Player *player);
void PlayerDraw(SDL_Renderer *renderer, SDL_Window *window, Player *player);

void PlayerPlay(Player *player);
void PlayerStop(Player *player);

#endif // _PLAYER_H_
