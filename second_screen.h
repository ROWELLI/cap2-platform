// second_screen.h
#ifndef SECOND_SCREEN_H
#define SECOND_SCREEN_H

#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include "config_loader.h"

void drawSecondScreen(SDL_Renderer *renderer, const std::vector<std::string> &items, const Config &config);

#endif
