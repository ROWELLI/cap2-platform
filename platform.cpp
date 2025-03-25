// platform.cpp (drawSecondScreen 함수는 secondescreen.cpp로 이동, config에서 세컨드스크린 정보 가져오기)
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <filesystem>
#include "pixel_capture.h"
#include "config_loader.h"
#include "second_screen.h"

using namespace std;

int main(int argc, char *argv[])
{
    Config config;
    if (!loadConfig(config))
    {
        cerr << "설정 파일을 불러오지 못했습니다. 기본값을 사용합니다." << endl;
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0 || TTF_Init() != 0)
    {
        cerr << "초기화 실패: " << SDL_GetError() << endl;
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) & (IMG_INIT_JPG | IMG_INIT_PNG)))
    {
        cerr << "SDL_image 초기화 실패: " << IMG_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("SDL2 UI",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          config.SCREEN_WIDTH, config.SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!window || !renderer)
    {
        cerr << "윈도우 또는 렌더러 생성 실패" << endl;
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    TTF_Font *font = TTF_OpenFont(config.FONT_PATH.c_str(), config.FONT_SIZE);
    if (!font)
    {
        cerr << "폰트 로드 실패: " << TTF_GetError() << endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Texture *backgroundTexture = nullptr;
    SDL_Surface *bgSurface = IMG_Load(config.BACKGROUND_IMAGE.c_str());
    if (!bgSurface)
    {
        cerr << "배경 이미지 로드 실패: " << config.BACKGROUND_IMAGE << " - " << IMG_GetError() << endl;
    }
    else
    {
        backgroundTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
        SDL_FreeSurface(bgSurface);
    }

    int actualCount = config.IMAGE_FILES.size();
    vector<SDL_Texture *> imageTextures(actualCount, nullptr);
    for (int i = 0; i < actualCount; i++)
    {
        SDL_Surface *imgSurface = IMG_Load(config.IMAGE_FILES[i].c_str());
        if (!imgSurface)
        {
            cerr << "이미지 로드 실패: " << config.IMAGE_FILES[i] << " - " << IMG_GetError() << endl;
            imageTextures[i] = nullptr;
        }
        else
        {
            imageTextures[i] = SDL_CreateTextureFromSurface(renderer, imgSurface);
            SDL_FreeSurface(imgSurface);
        }
    }

    allocatePixelMemory(config.SCREEN_WIDTH, config.SCREEN_HEIGHT);

    int selected = 0;
    float currentOffset = 0;
    vector<float> currentScale(actualCount, 1.0f);

    bool running = true;
    bool inMainView = true;

    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = false;

            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    if (!inMainView)
                        inMainView = true;
                    else
                        running = false;
                }
                else if (inMainView)
                {
                    if (event.key.keysym.sym == SDLK_LEFT)
                        selected = (selected - 1 + actualCount) % actualCount;
                    else if (event.key.keysym.sym == SDLK_RIGHT)
                        selected = (selected + 1) % actualCount;
                    else if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER)
                        inMainView = false;
                }
            }
        }

        if (!inMainView)
        {
            std::string category = config.secondScreenMapping[selected];
            const std::vector<std::string> &contents = config.secondScreenContent[category];
            drawSecondScreen(renderer, contents, config);
            SDL_Delay(16);
            continue;
        }

        currentOffset += (selected - currentOffset) * config.ANIMATION_SPEED;
        for (int i = 0; i < actualCount; i++)
        {
            if (i == selected)
                currentScale[i] += (config.SELECTED_SCALE - currentScale[i]) * config.SCALE_SPEED;
            else
                currentScale[i] += (1.0 - currentScale[i]) * config.SCALE_SPEED;
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        if (backgroundTexture)
        {
            SDL_Rect bgRect = {0, 0, config.SCREEN_WIDTH, config.SCREEN_HEIGHT};
            SDL_RenderCopy(renderer, backgroundTexture, NULL, &bgRect);
        }

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        int centerX = config.SCREEN_WIDTH / 2 - (config.RECT_WIDTH * config.SELECTED_SCALE) / 2;
        int startY = config.SCREEN_HEIGHT - config.RECT_HEIGHT - config.BOTTOM_MARGIN;
        int startX = centerX - currentOffset * (config.RECT_WIDTH + config.RECT_GAP);

        for (int i = 0; i < actualCount; i++)
        {
            int rectWidth = config.RECT_WIDTH * currentScale[i];
            int rectHeight = config.RECT_HEIGHT * currentScale[i];
            int offsetY = (rectHeight - config.RECT_HEIGHT) / 2;

            SDL_Rect rect = {startX, startY - offsetY, rectWidth, rectHeight};

            if (!imageTextures[i])
            {
                SDL_SetRenderDrawColor(renderer,
                                       i == selected ? 255 : 0,
                                       0,
                                       i == selected ? 0 : 255,
                                       255);
                SDL_RenderFillRect(renderer, &rect);
            }

            if (imageTextures[i])
            {
                SDL_Rect imgRect = {startX, startY - offsetY, rectWidth, rectHeight};
                SDL_RenderCopy(renderer, imageTextures[i], NULL, &imgRect);
            }

            startX += rectWidth + config.RECT_GAP;
        }

        capturePixels(renderer, config.SCREEN_WIDTH, config.SCREEN_HEIGHT);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    for (SDL_Texture *texture : imageTextures)
        if (texture)
            SDL_DestroyTexture(texture);

    if (backgroundTexture)
        SDL_DestroyTexture(backgroundTexture);

    freePixelMemory();
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}