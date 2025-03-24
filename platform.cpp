#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include "pixel_capture.h"
#include "config_loader.h"

using namespace std;
int main(int argc, char *argv[])
{
    // 설정 파일 로드
    Config config;
    if (!loadConfig(config))
    {
        cerr << "설정 파일을 불러오지 못했습니다. 기본값을 사용합니다." << endl;
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

    SDL_Window *window = SDL_CreateWindow(
        "SDL2 UI",
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

    // **이미지 로드**
    vector<SDL_Texture *> imageTextures(config.RECT_COUNT, nullptr);
    for (int i = 0; i < config.RECT_COUNT; i++)
    {
        SDL_Surface *imgSurface = IMG_Load(config.IMAGE_FILES[i].c_str());
        if (!imgSurface)
        {
            cerr << "이미지 로드 실패: " << config.IMAGE_FILES[i] << " - " << IMG_GetError() << endl;
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
    vector<float> currentScale(config.RECT_COUNT, 1.0f);

    bool running = true;
    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_LEFT)
                {
                    selected = (selected - 1 + config.RECT_COUNT) % config.RECT_COUNT;
                }
                else if (event.key.keysym.sym == SDLK_RIGHT)
                {
                    selected = (selected + 1) % config.RECT_COUNT;
                }
            }
        }

        currentOffset += (selected - currentOffset) * config.ANIMATION_SPEED;

        for (int i = 0; i < config.RECT_COUNT; i++)
        {
            if (i == selected)
            {
                currentScale[i] += (config.SELECTED_SCALE - currentScale[i]) * config.SCALE_SPEED;
            }
            else
            {
                currentScale[i] += (1.0 - currentScale[i]) * config.SCALE_SPEED;
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // **SPHERE 글씨 추가**
        SDL_Color textColor = {0, 0, 0, 255};
        SDL_Surface *textSurface = TTF_RenderText_Solid(font, "SPHERE", textColor);
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        int textWidth = textSurface->w;
        int textHeight = textSurface->h;
        SDL_Rect textRect = {(config.SCREEN_WIDTH - textWidth) / 2, 20, textWidth, textHeight};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);

        // **박스 및 이미지 렌더링**
        int centerX = config.SCREEN_WIDTH / 2 - (config.RECT_WIDTH * config.SELECTED_SCALE) / 2;
        int startY = config.SCREEN_HEIGHT - config.RECT_HEIGHT - config.BOTTOM_MARGIN;
        int startX = centerX - currentOffset * (config.RECT_WIDTH + config.RECT_GAP);

        for (int i = 0; i < config.RECT_COUNT; i++)
        {
            int rectWidth = config.RECT_WIDTH * currentScale[i];
            int rectHeight = config.RECT_HEIGHT * currentScale[i];
            int offsetY = (rectHeight - config.RECT_HEIGHT) / 2;

            if (i == selected)
            {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            }
            else
            {
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            }

            SDL_Rect rect = {startX, startY - offsetY, rectWidth, rectHeight};
            SDL_RenderFillRect(renderer, &rect);

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

    // **메모리 해제**
    for (SDL_Texture *texture : imageTextures)
    {
        if (texture)
        {
            SDL_DestroyTexture(texture);
        }
    }

    freePixelMemory();
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
