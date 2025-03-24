#include "pixel_capture.h"
#include "image_processor/image_processor.h"
#include "packet_sender/e131_sender.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <cstring>

using namespace std;

unsigned char *pixels = nullptr;

void allocatePixelMemory(int screen_width, int screen_height)
{
    if (pixels)
        delete[] pixels;
    pixels = new unsigned char[screen_width * screen_height * 3];
}

void freePixelMemory()
{
    delete[] pixels;
    pixels = nullptr;
}

void capturePixels(SDL_Renderer *rR, int screen_width, int screen_height)
{
    if (!pixels)
    {
        cerr << "픽셀 메모리가 할당되지 않았습니다!" << endl;
        return;
    }

    static ImageProcessor *image = nullptr;
    static E131Sender *sender = nullptr;

    if (!image)
    {
        try
        {
            int rows[54] = {/* ... (생략된 rows 값) ... */};
            image = new ImageProcessor(screen_height, screen_width, 0, 0, 359, 0, rows, 54);
        }
        catch (exception &e)
        {
            cerr << "ImageProcessor 생성 실패: " << e.what() << endl;
            return;
        }
    }

    if (!sender)
    {
        try
        {
            const char *ip = "192.168.50.72";
            sender = new E131Sender(ip);
        }
        catch (exception &e)
        {
            cerr << "Sender 생성 실패: " << e.what() << endl;
            return;
        }
    }

    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, screen_width, screen_height, 24, SDL_PIXELFORMAT_RGB24);
    if (!surface || !surface->pixels)
    {
        cerr << "Surface 생성 실패: " << SDL_GetError() << endl;
        return;
    }

    if (SDL_RenderReadPixels(rR, NULL, SDL_PIXELFORMAT_RGB24, surface->pixels, surface->pitch) != 0)
    {
        cerr << "픽셀 캡처 실패: " << SDL_GetError() << endl;
        SDL_FreeSurface(surface);
        return;
    }

    unsigned char *surfacePixels = static_cast<unsigned char *>(surface->pixels);
    for (int y = 0; y < screen_height; y++)
    {
        for (int x = 0; x < screen_width; x++)
        {
            int src_idx = y * surface->pitch + x * 3;
            int dst_idx = (y * screen_width + x) * 3;
            pixels[dst_idx] = surfacePixels[src_idx + 2];
            pixels[dst_idx + 1] = surfacePixels[src_idx + 1];
            pixels[dst_idx + 2] = surfacePixels[src_idx];
        }
    }

    image->mask(pixels);
    image->rotate();
    sender->send(image->get_processed_image(), 48771);
    sender->next();

    SDL_FreeSurface(surface);
}
