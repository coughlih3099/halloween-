// CSC 134
// Halloween++
// Harley Coughlin
// 10/22/2024
//
// Made consulting Claude.ai
// Also followed a tutorial by Erik Yuzwa
//  https://www.youtube.com/watch?v=vjU62r-M1CY&t=3595s

#include <raylib.h>
#include <cassert>
#include "../include/main.hpp"


void GameStartup();
void GameUpdate();
void GameRender();
void GameShutdown();

// ------------------

Texture2D textures[MAX_TEXTURES];

// create array for storing map
Tile world[WORLD_WIDTH][WORLD_HEIGHT];

/////////////////////////////

Camera2D camera = { 0 };

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

// -----------------


int main() {
    // Init
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "HPP");
    SetTargetFPS(60);

    GameStartup();
    // ----

    // Main Loop
    while (!WindowShouldClose()) {
        // Update
        // ------
        GameUpdate();

        // Draw
        // ----
        BeginDrawing();
        ClearBackground(GRAY);

        GameRender();

        EndDrawing();
    }

    // De-Init
    // -------
    GameShutdown();

    CloseWindow();
    return 0;
}


void GameStartup() {
    // InitAudioDevice();

    Image image = LoadImage("../assets/colored_packed.png");
    textures[TEXTURE_TILEMAP] = LoadTextureFromImage(image);
    UnloadImage(image);


    // Populate the world array with tiles
    for (int w = 0; w < WORLD_WIDTH; w++) {
        for (int h = 0; h < WORLD_HEIGHT; h++) {
            world[w][h] = (Tile) {
                .position = (Vector2) { static_cast<float>(w),
                                        static_cast<float>(h) },
                .type = static_cast<decltype(Tile::type)>
                (GetRandomValue(Tile::DIRT, Tile::TREE))
            };
        }
    }
}
