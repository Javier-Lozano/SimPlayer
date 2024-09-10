#ifndef _VISUAL_H_
#define _VISUAL_H_

#include <stdbool.h>
#include "SDL.h"
#include "SDL_ttf.h"

#define BORDER (32)
#define V_PI   (3.1416f)
#define V_2PI  (2 * V_PI)

typedef struct player_st Player;

extern TTF_Font *g_Font;

bool VisualInit(SDL_Renderer *renderer);
void VisualClose();

void VisualMain(SDL_Window *window, SDL_Renderer *renderer, Player *player, float *stream);
void VisualDemo(SDL_Window *window, SDL_Renderer *renderer, Player *player, float radians);
void VisualWaves(SDL_Window *window, SDL_Renderer *renderer, Player *player, float *stream);

#endif // _VISUAL_H_

