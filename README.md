g++ platform.cpp pixel_capture.cpp include/image_processor/image_processor.cpp include/packet_sender/e131_sender.cpp -o sdl_ui \
    -I./include \
    -lSDL2 -lSDL2main -lSDL2_ttf -lSDL2_image -ljsoncpp -le131
