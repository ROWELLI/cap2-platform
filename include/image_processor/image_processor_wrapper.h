#ifndef IMAGE_PROCESSOR_WRAPPER_H
#define IMAGE_PROCESSOR_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// Opaque struct to hide C++ class details from C
typedef struct ImageProcessorWrapper ImageProcessorWrapper;

// Creates an ImageProcessor instance
ImageProcessorWrapper* create_image_processor(int height, int width, int tpad, int sratio, int eratio, int dpad, int* rows, int row_size);

// Processes image (modify this function if there's a specific processing method)
void process_image(ImageProcessorWrapper* processor);

// Destroys the ImageProcessor instance
void destroy_image_processor(ImageProcessorWrapper* processor);

#ifdef __cplusplus
}
#endif

#endif // IMAGE_PROCESSOR_WRAPPER_H

