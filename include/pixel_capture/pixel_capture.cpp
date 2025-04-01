#include "pixel_capture.h"
#include "image_processor/image_processor.h"
#include "packet_sender/e131_sender.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <cstring>

using namespace std;

// Global pixel buffer pointer
unsigned char *pixels = nullptr;

// Allocate memory for pixel buffer (RGB format)
void allocatePixelMemory(int screen_width, int screen_height)
{
    if (pixels)
        delete[] pixels;
    pixels = new unsigned char[screen_width * screen_height * 3];
}

// Free previously allocated pixel memory
void freePixelMemory()
{
    delete[] pixels;
    pixels = nullptr;
}

// Capture screen pixels and process them
void capturePixels(SDL_Renderer *rR, int screen_width, int screen_height)
{
    if (!pixels)
    {
        cerr << "Pixel memory is not allocated!" << endl;
        return;
    }

    // Static instances to persist across multiple calls
    static ImageProcessor *image = nullptr;
    static E131Sender *sender = nullptr;

    // Initialize ImageProcessor only once
    if (!image)
    {
        try
        {
            int rows[54] = {/* ... (omitted for brevity) ... */};
            image = new ImageProcessor(screen_height, screen_width, 0, 0, 359, 0, rows, 54);
        }
        catch (exception &e)
        {
            cerr << "Failed to create ImageProcessor: " << e.what() << endl;
            return;
        }
    }

    // Initialize E1.31 sender only once
    if (!sender)
    {
        try
        {
            const char *ip = "192.168.50.72";
            sender = new E131Sender(ip);
        }
        catch (exception &e)
        {
            cerr << "Failed to create sender: " << e.what() << endl;
            return;
        }
    }

    // Create a surface to capture pixels in RGB24 format
    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, screen_width, screen_height, 24, SDL_PIXELFORMAT_RGB24);
    if (!surface || !surface->pixels)
    {
        cerr << "Failed to create surface: " << SDL_GetError() << endl;
        return;
    }

    // Read pixels from the renderer into the surface
    if (SDL_RenderReadPixels(rR, NULL, SDL_PIXELFORMAT_RGB24, surface->pixels, surface->pitch) != 0)
    {
        cerr << "Failed to read pixels: " << SDL_GetError() << endl;
        SDL_FreeSurface(surface);
        return;
    }

    // Convert pixel format from surface to internal buffer (convert RGB -> BGR)
    unsigned char *surfacePixels = static_cast<unsigned char *>(surface->pixels);
    for (int y = 0; y < screen_height; y++)
    {
        for (int x = 0; x < screen_width; x++)
        {
            int src_idx = y * surface->pitch + x * 3;
            int dst_idx = (y * screen_width + x) * 3;
            pixels[dst_idx] = surfacePixels[src_idx + 2];     // Red
            pixels[dst_idx + 1] = surfacePixels[src_idx + 1]; // Green
            pixels[dst_idx + 2] = surfacePixels[src_idx];     // Blue
        }
    }

    // Apply image masking and rotation
    image->mask(pixels);
    image->rotate();

    // Send processed image using E1.31 protocol
    sender->send(image->get_processed_image(), 48771);
    sender->next();

    // Free the temporary surface
    SDL_FreeSurface(surface);
}
