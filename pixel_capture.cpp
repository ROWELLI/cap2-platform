#include "pixel_capture.h"
#include "image_processor/image_processor.h"
#include "packet_sender/e131_sender.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <unistd.h>
#include <cstring>

using namespace std;

// 전역 픽셀 버퍼
unsigned char *pixels = nullptr;

// static 객체는 capturePixels 내부에 존재
void allocatePixelMemory(int screen_width, int screen_height)
{
    pixels = new unsigned char[screen_width * screen_height * 3];
}

void freePixelMemory()
{
    delete[] pixels;
    pixels = nullptr;
}

// 캡처 및 전송
void capturePixels(SDL_Renderer *rR, int screen_width, int screen_height)
{
    if (!pixels)
    {
        cerr << "픽셀 메모리가 할당되지 않았습니다!" << endl;
        return;
    }

    // 🔁 한 번만 생성되는 정적 객체
    static ImageProcessor *image = nullptr;
    static E131Sender *sender = nullptr;

    if (!image)
    {
        int rows[54] = {
            104, 126, 141, 157, 172, 187, 199, 211, 224, 235, 246, 255, 263, 272, 279, 286, 293, 299,
            306, 311, 316, 320, 326, 331, 335, 337, 342, 345, 347, 351, 353, 355, 357, 356, 358, 359,
            359, 359, 359, 358, 358, 356, 355, 353, 351, 348, 345, 342, 338, 334, 330, 325, 319, 314};
        image = new ImageProcessor(screen_height, screen_width, 0, 0, 359, 0, rows, 54);
    }

    if (!sender)
    {
        const char *ip = "192.168.50.72";
        sender = new E131Sender(ip);
    }

    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, screen_width, screen_height, 24, SDL_PIXELFORMAT_RGB24);
    if (!surface)
    {
        cerr << "Failed to create surface: " << SDL_GetError() << endl;
        return;
    }

    if (SDL_RenderReadPixels(rR, NULL, SDL_PIXELFORMAT_RGB24, surface->pixels, surface->pitch) != 0)
    {
        cerr << "Failed to read pixels: " << SDL_GetError() << endl;
        SDL_FreeSurface(surface);
        return;
    }

    // RGB → BGR 변환 후 저장
    unsigned char *surfacePixels = static_cast<unsigned char *>(surface->pixels);
    for (int y = 0; y < screen_height; y++)
    {
        for (int x = 0; x < screen_width; x++)
        {
            int src_idx = y * surface->pitch + x * 3;
            int dst_idx = (y * screen_width + x) * 3;
            pixels[dst_idx] = surfacePixels[src_idx + 2];     // B
            pixels[dst_idx + 1] = surfacePixels[src_idx + 1]; // G
            pixels[dst_idx + 2] = surfacePixels[src_idx];     // R
        }
    }

    image->mask(pixels);
    image->rotate();

    sender->send(image->get_processed_image(), 48771); // 전송
    sender->next();

    SDL_FreeSurface(surface);
}
