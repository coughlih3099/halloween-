# How to play
You can walk using the arrow keys, and bumping into the enemies 3 times will kill them.

## How to compile on Linux
- cd to halloweenpp folder
- run: clang++ -std=c++17 -o game.out src/*.cpp -I./include -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

## How to compile on Windows
- install raylib
- go to the install location, then w64devkit
- double click w64devkit.exe
- cd to halloweenpp folder
- run: g++ -o game.exe src/\*.cpp -Iinclude/\*.hpp -lraylib -lgdi32 -lwinmm
