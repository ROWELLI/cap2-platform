# SDL UI Application

This project is an SDL-based UI interface for rendering and selecting items with image previews, animations, and optional command execution. It utilizes `SDL2`, `SDL2_image`, `SDL2_ttf`, `jsoncpp`, and `libe131`.

## 🧪 Build & Run

### 🔧 Manual Build

To compile manually using `g++`:

```
g++ platform.cpp include/pixel_capture/pixel_capture.cpp include/image_processor/image_processor.cpp include/packet_sender/e131_sender.cpp -o sdl_ui \
    -I./include \
    -lSDL2 -lSDL2main -lSDL2_ttf -lSDL2_image -ljsoncpp -le131
./sdl_ui
```
### 📦 Using Makefile
```
make
./sdl_ui
```

## 📥 Dependencies (Ubuntu)
```
sudo apt update
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libjsoncpp-dev
```

## 🎮 Features
- SDL-based user interface   
- Image-based item selection with animation   
- Keyboard interaction (←, →, Enter, Esc)   
- Optional system command execution per item   
- Background image and smooth scaling effects   