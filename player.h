#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "SDL.h"
#include "tracklist.h"

#define CHANNELS_MAX (8)
#define SAMPLES  (1024)

typedef enum {
	STATE_STOP         = 0x01,
	STATE_PLAY         = 0x02,
	STATE_PAUSE        = 0x04,
	STATE_STARUP       = 0x11,
	STATE_CLEANUP      = 0x21,
	STATE_END_OF_TRACK = 0x31,
} PlayerState;

typedef enum {
	MODE_NORMAL,
	MODE_LOOP_1,
	MODE_LOOP_ALL,
	MODE_RANDOM
} PlayerPlayMode;

typedef struct player_st {
	Uint8       is_running;
	Uint32      position;
	Tracklist   tracklist;
	TrackNode   *current;
	PlayerState state;
} Player;

void PlayerUpdate(Player *player);
void PlayerDraw(Player *player, SDL_Renderer *renderer);
void PlayerPrepare(Player *player);


#endif // _PLAYER_H_
