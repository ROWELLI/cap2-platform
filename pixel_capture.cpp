#include "pixel_capture.h"
#include "image_processor.h"
#include "e131_sender.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <unistd.h> // usleep()
#include <cstring>  // memcpy()

using namespace std;

// 픽셀 데이터를 저장할 전역 배열 포인터
unsigned char *pixels = nullptr;

// ImageProcessor 및 E1.31 Sender 객체 선언 (전역 변수)
ImageProcessor *imgProcessor = nullptr;
E131Sender *sender = nullptr;

// 메모리 할당 함수
void allocatePixelMemory(int screen_width, int screen_height)
{
    pixels = new unsigned char[screen_width * screen_height * 3]; // R, G, B 저장
}

// 메모리 해제 함수
void freePixelMemory()
{
    delete[] pixels;
    pixels = nullptr;

    // ImageProcessor 객체 해제
    delete imgProcessor;
    imgProcessor = nullptr;

    // E1.31 Sender 해제
    delete sender;
    sender = nullptr;
}

// 픽셀 데이터를 캡처하고 E1.31로 전송하는 함수
void capturePixels(SDL_Renderer *rR, int screen_width, int screen_height)
{
    if (!pixels)
    {
        cerr << "픽셀 메모리가 할당되지 않았습니다!" << endl;
        return;
    }

    int rows[54] = {
        104, 126, 141, 157, 172, 187, 199, 211, 224, 235, 246, 255, 263, 272, 279, 286, 293, 299,
        306, 311, 316, 320, 326, 331, 335, 337, 342, 345, 347, 351, 353, 355, 357, 356, 358, 359,
        359, 359, 359, 358, 358, 356, 355, 353, 351, 348, 345, 342, 338, 334, 330, 325, 319, 314};
    const char *ip = "192.168.50.72";

    ImageProcessor *image = new ImageProcessor(screen_height, screen_width, 0, 0, 359, 0, rows, 54);
    E131Sender *sender = new E131Sender(ip);

    Uint32 frameTime = SDL_GetTicks();
    int width, height;
    SDL_GetRendererOutputSize(rR, &width, &height);

    // 🎯 `SDL_CreateRGBSurfaceWithFormat()`을 루프 내에서 생성
    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 24, SDL_PIXELFORMAT_RGB24);
    if (!surface)
    {
        cerr << "Failed to create surface: " << SDL_GetError() << endl;
        return;
    }

    // 🎯 픽셀 데이터 캡처 (`SDL_RenderReadPixels`)
    if (SDL_RenderReadPixels(rR, NULL, SDL_PIXELFORMAT_RGB24, surface->pixels, surface->pitch) != 0)
    {
        cerr << "Failed to read pixels: " << SDL_GetError() << endl;
        SDL_FreeSurface(surface);
        return;
    }

    // 🎯 RGB → BGR 변환 추가
    unsigned char *surfacePixels = static_cast<unsigned char *>(surface->pixels);
    for (int y = 0; y < screen_height; y++)
    {
        for (int x = 0; x < screen_width; x++)
        {
            int pixel_index = (y * surface->pitch) + (x * 3); // 🎯 24비트 픽셀 데이터 (RGB 순서)

            unsigned char r = surfacePixels[pixel_index];     // 🔹 Red 값 (원래 위치)
            unsigned char g = surfacePixels[pixel_index + 1]; // 🔹 Green 값
            unsigned char b = surfacePixels[pixel_index + 2]; // 🔹 Blue 값 (원래 위치)

            // 🎯 RGB → BGR 변환
            int index = (y * screen_width + x) * 3;
            pixels[index] = b;     // 🔹 Blue → B 위치
            pixels[index + 1] = g; // 🔹 Green 그대로 유지
            pixels[index + 2] = r; // 🔹 Red → R 위치
        }
    }

    // 🎯 ImageProcessor에 픽셀 데이터 전달
    image->mask(pixels);
    image->rotate();

    // 🎯 처리된 이미지 데이터를 E1.31 전송
    sender->send(image->get_processed_image(), 48771);
    sender->next();

    SDL_FreeSurface(surface);

    // 🎯 객체 삭제
    delete image;
    delete sender;
}