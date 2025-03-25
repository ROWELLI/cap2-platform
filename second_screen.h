#ifndef SECOND_SCREEN_H
#define SECOND_SCREEN_H

#include <SDL2/SDL.h>
#include <string>
#include <map>
#include "config_loader.h"

void drawSecondScreen(SDL_Renderer *renderer, const std::string *items, int itemCount, const Config &config);

#endif // SECOND_SCREEN_H