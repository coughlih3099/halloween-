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
#include <optional>
#include "../include/main.hpp"
#include "../include/entitysystem.hpp"


void GameStartup();
void GameUpdate();
void GameRender();
void GameShutdown();
void DrawTile(Position pos, Position to_draw);

// ------------------

Texture2D textures[MAX_TEXTURES];

Tile world[WORLD_WIDTH][WORLD_HEIGHT];

const Position sprite_player = { 26, 0 };
const Position tile_dirt = { 2, 0 };
const Position tile_grass = { 5, 0 };
const Position tile_tree = { 0, 1 };
const Position tile_stone = { 5, 2 };

/////////////////////////////

Camera2D camera = { 0 };

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

// -----------------

EntityData entities;

// track the player between phases
int player_idx = -1;

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

    Image image = LoadImage("./assets/colored_packed.png");
    textures[TEXTURE_TILEMAP] = LoadTextureFromImage(image);
    UnloadImage(image);


    // Populate the world array with tiles
    for (int w = 0; w < WORLD_WIDTH; w++) {
        for (int h = 0; h < WORLD_HEIGHT; h++) {
            // get random tile type
            enum Tile::type random_type = static_cast<enum Tile::type>(
                GetRandomValue(Tile::DIRT, Tile::STONE));

            world[w][h] = (Tile) {
                .position = (Position) { w, h },
                .type = random_type,
                .traversable = (random_type == Tile::DIRT ||
                                random_type == Tile::GRASS) ? true : false
            };
        }
    }

    EntitySystem::initialize(&entities);

    Position initial_position_player = { 3, 3 };

    // overwrite the tile the player is supposed to spawn on with a guaranteed
    // traversable tile
    world[initial_position_player.x][initial_position_player.y].type = Tile::DIRT;
    world[initial_position_player.x][initial_position_player.y].traversable = true;

    // create the player
    auto player = EntitySystem::create_entity(&entities, world,
                                              initial_position_player);
    if (!player.has_value()) {
        TraceLog(LOG_ERROR, "Failed to create player entity!");
        TraceLog(LOG_ERROR, "World[%d][%d]: type=%d",
                 initial_position_player.x, initial_position_player.y,
                 world[initial_position_player.x][initial_position_player.y].type);
        return;
    }
    player_idx = player.value();
    int player_pos_x = entities.positions[player_idx].x;
    int player_pos_y = entities.positions[player_idx].y;


    // Init the camera
    if (player_idx >= 0 && player_idx < MAX_ENTITIES) {
        camera.target = (Vector2){ static_cast<float>(player_pos_x),
                                   static_cast<float>(player_pos_y) };
        camera.offset = (Vector2){ static_cast<float>(WINDOW_WIDTH) / 2,
                                   static_cast<float>(WINDOW_HEIGHT) / 2 };
        camera.zoom = 3.0f;
    } else {
        TraceLog(LOG_ERROR, "Invalid player index!");
    }
}


void DrawTile(Position pos, Position to_draw) {
    Rectangle source = {
        static_cast<float>(to_draw.x * TILE_WIDTH),
        static_cast<float>(to_draw.y * TILE_HEIGHT),
        static_cast<float>(TILE_WIDTH),
        static_cast<float>(TILE_HEIGHT)
    };
    Rectangle dest = {
        static_cast<float>(pos.x),
        static_cast<float>(pos.y),
        static_cast<float>(TILE_HEIGHT),
        static_cast<float>(TILE_HEIGHT)
    };
    Vector2 origin = { 0, 0 };
    DrawTexturePro(textures[TEXTURE_TILEMAP], source, dest, origin, 0.0f,
                   WHITE);
}


void GameUpdate() {
    // Move the player on key press
    switch (GetKeyPressed()) {
        case KEY_UP:
            EntitySystem::move_entity(&entities, world, player_idx, NORTH);
            break;
        case KEY_LEFT:
            EntitySystem::move_entity(&entities, world, player_idx, EAST);
            break;
        case KEY_DOWN:
            EntitySystem::move_entity(&entities, world, player_idx, SOUTH);
            break;
        case KEY_RIGHT:
            EntitySystem::move_entity(&entities, world, player_idx, WEST);
            break;
    }

    // follow the player with the camera
    camera.target = (Vector2) {
        static_cast<float>(entities.positions[player_idx].x * TILE_WIDTH),
        static_cast<float>(entities.positions[player_idx].y * TILE_HEIGHT),
    };
}


void GameRender() {
    BeginMode2D(camera);

    // Draw the map
    Tile tile;
    Position to_draw;
    for (int w = 0; w < WORLD_WIDTH; w++) {
        for (int h = 0; h < WORLD_HEIGHT; h++) {
            tile = world[w][h];
            switch (tile.type) {
                case Tile::DIRT:
                    to_draw = tile_dirt;
                    break;
                case Tile::GRASS:
                    to_draw = tile_grass;
                    break;
                case Tile::TREE:
                    to_draw = tile_tree;
                    break;
                case Tile::STONE:
                    to_draw = tile_stone;
                    break;
            }
            Position pos = tile.position *
                           (Position){ TILE_WIDTH, TILE_HEIGHT };
            DrawTile(pos, to_draw);
        }
    }
    // Draw the player
    DrawTile({static_cast<int>(camera.target.x),
              static_cast<int>(camera.target.y)}, sprite_player);

    EndMode2D();
}


void GameShutdown() {
    for (int i = 0; i < MAX_TEXTURES; i++) {
        UnloadTexture(textures[i]);
    }

    // CloseAudioDevice();
}
