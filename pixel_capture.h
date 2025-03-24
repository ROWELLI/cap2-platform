#ifndef PIXEL_CAPTURE_H
#define PIXEL_CAPTURE_H

#include <SDL2/SDL.h>

extern unsigned char* pixels;  // 픽셀 데이터를 저장할 전역 배열

void allocatePixelMemory(int screen_width, int screen_height);
void freePixelMemory();
void capturePixels(SDL_Renderer* renderer, int screen_width, int screen_height);

#endif // PIXEL_CAPTURE_H
