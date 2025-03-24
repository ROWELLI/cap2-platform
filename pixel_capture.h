#ifndef PIXEL_CAPTURE_H
#define PIXEL_CAPTURE_H

#include <SDL2/SDL.h>

extern unsigned char *pixels;

void allocatePixelMemory(int screen_width, int screen_height);
void freePixelMemory();
void capturePixels(SDL_Renderer *rR, int screen_width, int screen_height);

#endif // PIXEL_CAPTURE_H
