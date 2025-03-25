#include "second_screen.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include "pixel_capture.h"
using namespace std;

void drawSecondScreen(SDL_Renderer *renderer, const std::string *items, int itemCount, const Config &config)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_Texture *textures[MAX_ITEMS] = {nullptr};
    for (int i = 0; i < itemCount; ++i)
    {
        std::string imagePath;
        for (const auto &categoryPair : config.secondScreenImageMap)
        {
            const auto &itemMap = categoryPair.second;
            auto it = itemMap.find(items[i]);
            if (it != itemMap.end())
            {
                imagePath = it->second;
                break;
            }
        }
        if (imagePath.empty())
            continue;

        SDL_Surface *surface = IMG_Load(imagePath.c_str());
        if (surface)
        {
            textures[i] = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);
        }
        else
        {
            std::cerr << "이미지 로드 실패: " << imagePath << " - " << IMG_GetError() << std::endl;
        }
    }

    float scale[MAX_ITEMS];
    for (int i = 0; i < itemCount; ++i)
        scale[i] = 1.0f;

    int centerX = config.SCREEN_WIDTH / 2 - (config.RECT_WIDTH * config.SELECTED_SCALE) / 2;
    int startY = config.SCREEN_HEIGHT - config.RECT_HEIGHT - config.BOTTOM_MARGIN;
    int totalWidth = itemCount * config.RECT_WIDTH + (itemCount - 1) * config.RECT_GAP;
    int startX = (config.SCREEN_WIDTH - totalWidth) / 2;

    for (int i = 0; i < itemCount; ++i)
    {
        int rectWidth = config.RECT_WIDTH * scale[i];
        int rectHeight = config.RECT_HEIGHT * scale[i];
        int offsetY = (rectHeight - config.RECT_HEIGHT) / 2;

        SDL_Rect rect = {startX, startY - offsetY, rectWidth, rectHeight};

        if (!textures[i])
        {
            SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255);
            SDL_RenderFillRect(renderer, &rect);
        }
        else
        {
            SDL_RenderCopy(renderer, textures[i], NULL, &rect);
        }

        startX += rectWidth + config.RECT_GAP;
    }

    for (int i = 0; i < itemCount; ++i)
        if (textures[i])
            SDL_DestroyTexture(textures[i]);

    capturePixels(renderer, config.SCREEN_WIDTH, config.SCREEN_HEIGHT);
    SDL_RenderPresent(renderer);
}