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
#include <vector>
#include "../include/main.hpp"
#include "../include/entitysystem.hpp"


void GameStartup();
void GameUpdate();
void GameRender();
void GameShutdown();
void DrawTile(Position position, Position to_draw);

// ------------------

Texture2D textures[MAX_TEXTURES];

Tile world[WORLD_WIDTH][WORLD_HEIGHT];

const Position TILE_SIZE = { TILE_WIDTH, TILE_HEIGHT };

const Position sprite_player = { 29, 6 };
const Position sprite_enemy = { 31, 2 };
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
int player_index = -1;

// track enemies between phases
std::vector<std::optional<int>>(enemy_handles);

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
                GetRandomValue(Tile::DIRT, Tile::TREE));

            world[w][h] = (Tile) {
                .position = (Position) { w, h },
                .type = random_type,
                .traversable = (random_type == Tile::DIRT ||
                                random_type == Tile::GRASS) ? true : false
            };
        }
    }

    EntitySystem::initialize(&entities);

    // init player info
    // TODO(coughlih3099): Change from hardcoded position
    Position initial_player_position = { 3, 3 };
    Stats initial_player_stats;
    initial_player_stats.attributes["Health"] = 100;
    initial_player_stats.attributes["Attack"] = 10;
    initial_player_stats.attributes["Defense"] = 10;

    // overwrite the tile the player is supposed to spawn on with a guaranteed
    // traversable tile
    world[initial_player_position.x][initial_player_position.y].type = Tile::DIRT;
    world[initial_player_position.x][initial_player_position.y].traversable = true;


    // create the player
    auto player = EntitySystem::create_entity(&entities, world,
                                              initial_player_position);
    if (player.has_value()) {
        entities.stats[player.value()] = initial_player_stats;
    }

    // Other entities starting stats
    Stats initial_enemy_stats;
    initial_enemy_stats.attributes["Health"] = 100;
    initial_enemy_stats.attributes["Attack"] = 10;
    initial_enemy_stats.attributes["Defense"] = 10;

    // create an amount of entities
    while (entities.dead_count > 0) {
        auto enemy_handle = EntitySystem::create_entity(&entities, world,
                                (Position) { GetRandomValue(0, WORLD_WIDTH),
                                             GetRandomValue(0, WORLD_HEIGHT) });
        if (enemy_handle.has_value()) {
            enemy_handles.push_back(enemy_handle);
            entities.stats[enemy_handle.value()] = initial_enemy_stats;
        }
    }

    if (!player.has_value()) {
        TraceLog(LOG_ERROR, "Failed to create player entity!");
        TraceLog(LOG_ERROR, "World[%d][%d]: type=%d",
                 initial_player_position.x, initial_player_position.y,
                 world[initial_player_position.x][initial_player_position.y].type);
        return;
    }
    player_index = player.value();
    int player_pos_x = entities.positions[player_index].x;
    int player_pos_y = entities.positions[player_index].y;


    // Init the camera
    if (player_index >= 0 && player_index < MAX_ENTITIES) {
        camera.target = (Vector2){ static_cast<float>(player_pos_x),
                                   static_cast<float>(player_pos_y) };
        camera.offset = (Vector2){ static_cast<float>(WINDOW_WIDTH) / 2,
                                   static_cast<float>(WINDOW_HEIGHT) / 2 };
        camera.zoom = 3.0f;
    } else {
        TraceLog(LOG_ERROR, "Invalid player index!");
    }
}


void DrawTile(Position position, Position to_draw) {
    Rectangle source = {
        static_cast<float>(to_draw.x * TILE_WIDTH),
        static_cast<float>(to_draw.y * TILE_HEIGHT),
        static_cast<float>(TILE_WIDTH),
        static_cast<float>(TILE_HEIGHT)
    };
    Rectangle dest = {
        static_cast<float>(position.x),
        static_cast<float>(position.y),
        static_cast<float>(TILE_HEIGHT),
        static_cast<float>(TILE_HEIGHT)
    };
    Vector2 origin = { 0, 0 };
    DrawTexturePro(textures[TEXTURE_TILEMAP], source, dest, origin, 0.0f,
                   WHITE);
}


void GameUpdate() {
    // Track if the player moved and which direction they tried to move
    bool player_moved = false;
    int key_pressed = KEY_NULL;
    Direction direction;
    // Move the player on key press
    switch (key_pressed = GetKeyPressed()) {
        case KEY_UP:
            direction = NORTH;
            player_moved = EntitySystem::move_entity(&entities, world,
                                                     player_index, direction);
            break;
        case KEY_LEFT:
            direction = EAST;
            player_moved = EntitySystem::move_entity(&entities, world,
                                                     player_index, direction);
            break;
        case KEY_DOWN:
            direction = SOUTH;
            player_moved = EntitySystem::move_entity(&entities, world,
                                                     player_index, direction);
            break;
        case KEY_RIGHT:
            direction = WEST;
            player_moved = EntitySystem::move_entity(&entities, world,
                                                     player_index, direction);
            break;
    }

    // if the player didn't move, try to attack
    if (!player_moved && key_pressed != KEY_NULL) {
        std::optional<int> damage_done = EntitySystem::attack(
            &entities, player_index, direction);
        if (damage_done.has_value()) {
            entities.stats[player_index]->attributes["Damage"] =
                damage_done.value();
            TraceLog(LOG_INFO, "Damage: %d",
                     entities.stats[player_index]->attributes["Damage"]);
        }
    }

    // follow the player with the camera
    camera.target = (Vector2) {
        static_cast<float>(entities.positions[player_index].x * TILE_WIDTH),
        static_cast<float>(entities.positions[player_index].y * TILE_HEIGHT),
    };


    for (auto enemy : enemy_handles) {
    if (entities.stats[enemy.value()]->attributes["Health"] <= 0) {
        EntitySystem::kill_entity(&entities, enemy.value());
        }
    }
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
            Position position = tile.position * TILE_SIZE;
            DrawTile(position, to_draw);
        }
    }
    for (int i = 0; i < entities.alive_count; i++) {
        auto index = entities.alive_indices[i];
        DrawTile(entities.positions[index] * TILE_SIZE, sprite_enemy);
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
