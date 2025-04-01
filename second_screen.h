#ifndef SECOND_SCREEN_H
#define SECOND_SCREEN_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include "config_loader.h"

using namespace std;

void secondScreenLoop(SDL_Renderer *renderer, const Config &config, const std::string &category)
{
    const auto &itemMap = config.secondScreenContent.at(category);

    string itemNames[MAX_ITEMS];
    string imagePaths[MAX_ITEMS];
    string commands[MAX_ITEMS];
    SDL_Texture *textures[MAX_ITEMS] = {nullptr};

    int itemCount = 0;
    for (const auto &entry : itemMap)
    {
        if (itemCount >= MAX_ITEMS)
            break;

        itemNames[itemCount] = entry.first;
        imagePaths[itemCount] = entry.second.first;
        commands[itemCount] = entry.second.second;

        SDL_Surface *imgSurface = IMG_Load(imagePaths[itemCount].c_str());
        if (imgSurface)
        {
            textures[itemCount] = SDL_CreateTextureFromSurface(renderer, imgSurface);
            SDL_FreeSurface(imgSurface);
        }

        itemCount++;
    }

    int selected = 0;
    float currentOffset = 0;
    float currentScale[MAX_ITEMS];
    for (int i = 0; i < itemCount; ++i)
        currentScale[i] = 1.0f;

    SDL_Texture *backgroundTexture = nullptr;
    SDL_Surface *bgSurface = IMG_Load(config.BACKGROUND_IMAGE.c_str());
    if (bgSurface)
    {
        backgroundTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
        SDL_FreeSurface(bgSurface);
    }

    bool inSecondScreen = true;
    while (inSecondScreen)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                inSecondScreen = false;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    inSecondScreen = false;
                }
                else if (event.key.keysym.sym == SDLK_LEFT)
                {
                    selected = (selected - 1 + itemCount) % itemCount;
                }
                else if (event.key.keysym.sym == SDLK_RIGHT)
                {
                    selected = (selected + 1) % itemCount;
                }
                else if (event.key.keysym.sym == SDLK_RETURN)
                {
                    if (!commands[selected].empty())
                    {
                        system(commands[selected].c_str());
                    }
                }
            }
        }

        currentOffset += (selected - currentOffset) * config.ANIMATION_SPEED;

        for (int i = 0; i < itemCount; ++i)
        {
            if (i == selected)
                currentScale[i] += (config.SELECTED_SCALE - currentScale[i]) * config.SCALE_SPEED;
            else
                currentScale[i] += (1.0f - currentScale[i]) * config.SCALE_SPEED;
        }

        SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
        SDL_RenderClear(renderer);

        if (backgroundTexture)
        {
            SDL_Rect bgRect = {0, 0, config.SCREEN_WIDTH, config.SCREEN_HEIGHT};
            SDL_RenderCopy(renderer, backgroundTexture, NULL, &bgRect);
        }

        int centerX = config.SCREEN_WIDTH / 2 - (config.RECT_WIDTH * config.SELECTED_SCALE) / 2;
        int startY = config.SCREEN_HEIGHT - config.RECT_HEIGHT - config.BOTTOM_MARGIN;
        int startX = centerX - currentOffset * (config.RECT_WIDTH + config.RECT_GAP);

        for (int i = 0; i < itemCount; ++i)
        {
            int rectWidth = config.RECT_WIDTH * currentScale[i];
            int rectHeight = config.RECT_HEIGHT * currentScale[i];
            int offsetY = (rectHeight - config.RECT_HEIGHT) / 2;
            SDL_Rect rect = {startX, startY - offsetY, rectWidth, rectHeight};

            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 180);
            SDL_RenderFillRect(renderer, &rect);

            if (textures[i])
            {
                SDL_RenderCopy(renderer, textures[i], NULL, &rect);
            }

            startX += rectWidth + config.RECT_GAP;
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    if (backgroundTexture)
        SDL_DestroyTexture(backgroundTexture);
    for (int i = 0; i < itemCount; ++i)
        if (textures[i])
            SDL_DestroyTexture(textures[i]);
}

#endif
