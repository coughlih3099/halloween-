## How to compile on Linux
- cd to halloweenpp folder
- run: clang++ -std=c++17 -o game src/main.cpp src/entitysystem.cpp -I./include -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

## How to compile on Windows
- install raylib
- go to the install location, then w64devkit
- double click w64devkit.exe
- cd to halloweenpp folder
- run: g++ -o game.exe src/\*.cpp -Iinclude/\*.hpp -lraylib -lgdi32 -lwinmm
