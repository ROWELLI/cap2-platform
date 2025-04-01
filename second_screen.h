#ifndef SECOND_SCREEN_H
#define SECOND_SCREEN_H

#include <SDL2/SDL.h>
#include <string>
#include "config_loader.h"

void secondScreenLoop(SDL_Renderer *renderer, const Config &config, const std::string &category);

#endif