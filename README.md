## 실행
g++ platform.cpp pixel_capture.cpp include/image_processor/image_processor.cpp include/packet_sender/e131_sender.cpp -o sdl_ui \
    -I./include \
    -lSDL2 -lSDL2main -lSDL2_ttf -lSDL2_image -ljsoncpp -le131

## Ubuntu 설치
sudo apt update
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libjsoncpp-dev