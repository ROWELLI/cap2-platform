#include "image_processor_wrapper.h"
#include "image_processor.h"

// Define the wrapper struct
struct ImageProcessorWrapper {
    ImageProcessor* instance;
};

// Creates an ImageProcessor instance
ImageProcessorWrapper* create_image_processor(int height, int width, int tpad, int sratio, int eratio, int dpad, int* rows, int row_size) {
    return new ImageProcessorWrapper{new ImageProcessor(height, width, tpad, sratio, eratio, dpad, rows, row_size)};
}

// Placeholder function for image processing (modify as needed)
void process_image(ImageProcessorWrapper* processor) {
    if (processor && processor->instance) {
        // Add processing functionality if required
    }
}

// Destroys an ImageProcessor instance
void destroy_image_processor(ImageProcessorWrapper* processor) {
    if (processor) {
        delete processor->instance;
        delete processor;
    }
}

