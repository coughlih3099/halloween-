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
#include "../include/ui.hpp"


void GameStartup();
void GameUpdate();
void GameRender();
void GameShutdown();
void DrawTile(Position position, Position to_draw);


// ------------------

Texture2D textures[MAX_TEXTURES];

Tile world[WORLD_WIDTH][WORLD_HEIGHT];

// const float viewport_top_edge = 48.0f;
// const float viewport_left_edge = 320.0f;
// const float viewport_right_edge = 960.0f;
// const float viewport_bottom_edge = 480.0f;
// const float viewport_width_middle = (viewport_right_edge - viewport_left_edge) / 2;
// const float viewport_length_middle = (viewport_bottom_edge - viewport_top_edge) / 2;
// const Vector2 VIEWPORT_MIDDLE = { viewport_width_middle + viewport_left_edge,
//                                   viewport_top_edge  + viewport_length_middle };

const Position TILE_SIZE = { TILE_WIDTH, TILE_HEIGHT };

// viewport measurements 80, 110, 290, 110;

const Position sprite_player = { 29, 6 };
const Position sprite_enemy = { 31, 2 };
const Position tile_dirt = { 2, 0 };
const Position tile_grass = { 5, 0 };
const Position tile_tree = { 0, 1 };
const Position tile_stone = { 5, 2 };

/////////////////////////////

Camera2D camera = { 0 };
Vector2 camera_target;

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

    Image ui_image = LoadImage("./assets/partui.png");
    textures[TEXTURE_UI] = LoadTextureFromImage(ui_image);
    UnloadImage(ui_image);


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
    initial_player_stats.attributes["Health"] = 30;
    initial_player_stats.attributes["Attack"] = 10;
    initial_player_stats.attributes["Defense"] = 10;

    // TODO(coughlih3099): Change from hardcoded position
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
    initial_enemy_stats.attributes["Health"] = 30;
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

    // Init the camera
    if (player_index >= 0 && player_index < MAX_ENTITIES) {
        camera.target = UI::get_clamped_camera(
                                            entities.positions[player_index]);
        camera.offset = UI::get_viewport_offset();
        camera.zoom = 4.5f;
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
    Direction move_direction = NONE;
    // Move the player on key press
    switch (key_pressed = GetKeyPressed()) {
        case KEY_UP:
            move_direction = NORTH;
            player_moved = EntitySystem::move_entity(&entities, world,
                                                     player_index, move_direction);
            break;
        case KEY_LEFT:
            move_direction = EAST;
            player_moved = EntitySystem::move_entity(&entities, world,
                                                     player_index, move_direction);
            break;
        case KEY_DOWN:
            move_direction = SOUTH;
            player_moved = EntitySystem::move_entity(&entities, world,
                                                     player_index, move_direction);
            break;
        case KEY_RIGHT:
            move_direction = WEST;
            player_moved = EntitySystem::move_entity(&entities, world,
                                                     player_index, move_direction);
            break;
    }

    // if the player didn't move, try to attack
    // should probably rework the attack function
    if (!player_moved && key_pressed != KEY_NULL && move_direction != NONE) {
        std::optional<int> damage_done = EntitySystem::attack(
            &entities, player_index, move_direction);
        if (damage_done.has_value()) {
            entities.stats[player_index]->attributes["Damage"] =
                damage_done.value();
            TraceLog(LOG_INFO, "Damage: %d",
                     entities.stats[player_index]->attributes["Damage"]);
        }
    }

    // move the camera to the updated player position, respecting viewport
    // boundaries
    camera.target = UI::get_clamped_camera(entities.positions[player_index]);

    // remove the enemy if it dies
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

    // Draw the enemies
    for (int i = entities.alive_count - 1; i > 0; i--) {
        auto index = entities.alive_indices[i];
        DrawTile(entities.positions[index] * TILE_SIZE, sprite_enemy);
    }

    // Draw the player
    DrawTile(entities.positions[player_index] * TILE_SIZE, sprite_player);

    EndMode2D();

    // Draw the UI texture
    DrawTexture(textures[TEXTURE_UI], 0, 0, WHITE);
}


void GameShutdown() {
    for (int i = 0; i < MAX_TEXTURES; i++) {
        UnloadTexture(textures[i]);
    }

    // CloseAudioDevice();
}
