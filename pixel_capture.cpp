// pixel_capture.cpp
#include "pixel_capture.h"
#include "image_processor/image_processor.h"
#include "packet_sender/e131_sender.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <cstring>

using namespace std;

unsigned char *pixels = nullptr;
ImageProcessor *imgProcessor = nullptr;
E131Sender *sender = nullptr;

int ROWS[54] = {
    104, 126, 141, 157, 172, 187, 199, 211, 224, 235, 246, 255, 263, 272, 279,
    286, 293, 299, 306, 311, 316, 320, 326, 331, 335, 337, 342, 345, 347, 351,
    353, 355, 357, 356, 358, 359, 359, 359, 359, 358, 358, 356, 355, 353, 351,
    348, 345, 342, 338, 334, 330, 325, 319, 314};
const int NUM_ROWS = 54;
const char *TARGET_IP = "192.168.50.72";

// 서피스를 재사용하기 위한 정적 변수
static SDL_Surface *captureSurface = nullptr;
static int frameCounter = 0;

void allocatePixelMemory(int screen_width, int screen_height)
{
    pixels = new unsigned char[screen_width * screen_height * 3];

    // 전역 객체 초기화 (한 번만 생성)
    if (!imgProcessor)
    {
        imgProcessor = new ImageProcessor(screen_height, screen_width, 0, 0, 359, 0, ROWS, NUM_ROWS);
    }
    if (!sender)
    {
        sender = new E131Sender(TARGET_IP);
    }
}

void freePixelMemory()
{
    delete[] pixels;
    pixels = nullptr;

    delete imgProcessor;
    imgProcessor = nullptr;

    delete sender;
    sender = nullptr;

    if (captureSurface)
    {
        SDL_FreeSurface(captureSurface);
        captureSurface = nullptr;
    }
}

void capturePixels(SDL_Renderer *renderer, int screen_width, int screen_height)
{
    // 프레임 제한: 2프레임에 1번만 처리
    frameCounter++;
    if (frameCounter % 2 != 0)
        return;

    if (!pixels || !imgProcessor || !sender)
    {
        cerr << "필수 메모리 또는 객체가 초기화되지 않았습니다!" << endl;
        return;
    }

    int width, height;
    SDL_GetRendererOutputSize(renderer, &width, &height);

    if (!captureSurface)
    {
        captureSurface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 24, SDL_PIXELFORMAT_RGB24);
        if (!captureSurface)
        {
            cerr << "서피스 생성 실패: " << SDL_GetError() << endl;
            return;
        }
    }

    if (SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGB24, captureSurface->pixels, captureSurface->pitch) != 0)
    {
        cerr << "픽셀 읽기 실패: " << SDL_GetError() << endl;
        return;
    }

    unsigned char *surfacePixels = static_cast<unsigned char *>(captureSurface->pixels);
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int pixel_index = (y * captureSurface->pitch) + (x * 3);
            unsigned char r = surfacePixels[pixel_index];
            unsigned char g = surfacePixels[pixel_index + 1];
            unsigned char b = surfacePixels[pixel_index + 2];

            int index = (y * width + x) * 3;
            pixels[index] = b;
            pixels[index + 1] = g;
            pixels[index + 2] = r;
        }
    }

    imgProcessor->mask(pixels);
    imgProcessor->rotate();
    sender->send(imgProcessor->get_processed_image(), 48771);
    sender->next();
}