g++ platform.cpp pixel_capture.cpp image_processor.cpp e131_sender.cpp -o sdl_ui \
    -I/mnt/c/Users/gram/Desktop/Capstone2/vcpkg/installed/x64-linux/include \
    -L/mnt/c/Users/gram/Desktop/Capstone2/vcpkg/installed/x64-linux/lib \
    -lSDL2 -lSDL2main -lSDL2_ttf -lSDL2_image -ljsoncpp -le131