#include "second_screen.h"
#include <SDL2/SDL_ttf.h>
#include <iostream>

void drawSecondScreen(SDL_Renderer *renderer, const std::string *items, int itemCount, const Config &config)
{
    SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
    SDL_RenderClear(renderer);

    TTF_Font *font = TTF_OpenFont(config.FONT_PATH.c_str(), config.FONT_SIZE / 2);
    if (!font)
    {
        std::cerr << "폰트 열기 실패: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Color color = {0, 0, 0, 255};
    int x = 100;
    int y = 100;

    for (int i = 0; i < itemCount; ++i)
    {
        SDL_Surface *surface = TTF_RenderText_Solid(font, items[i].c_str(), color);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect rect = {x, y, surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, NULL, &rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
        y += surface->h + 20;
    }

    TTF_CloseFont(font);
    SDL_RenderPresent(renderer);
}
