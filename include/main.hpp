#pragma once


#include <raylib.h>
#include <string>
#include <unordered_map>
#include <array>
#include <optional>


#define TILE_WIDTH 16.0  // float value because vector2 are floats
#define TILE_HEIGHT 16.0  // float value because vector2 are floats
#define WORLD_WIDTH 20
#define WORLD_HEIGHT 20
#define GRID_WIDTH WORLD_WIDTH
#define GRID_HEIGHT WORLD_HEIGHT
#define MAX_TEXTURES 1
#define MAX_ENTITIES 5  // TODO(coughlih3099): adjust this value


typedef enum Direction {
    NORTH,
    EAST,
    SOUTH,
    WEST
} direction;


typedef enum {
    TEXTURE_TILEMAP
} texture_asset;

// Definitions for the Map


/**
 * Holds tile information
 */
typedef struct Tile {
    Vector2 position;
    enum {
        DIRT,
        GRASS,
        TREE,
        STONE
    } type;
} Tile;


typedef enum {
    ZONE_ALL,
    ZONE_WORLD
} Zone;


// Definitions for characters


/**
 * Holds stat attributes
 */
typedef struct Stats {
    std::unordered_map<std::string, int> attributes;
} Stats;


/**
 * Holds type and stat attributes
 */
typedef struct Equipment {
    enum {
        WEAPON,
        ARMOR
    } type;
    Stats stats;
} Equipment;


typedef struct EntityData {
    // Character component data
    std::array<Vector2, MAX_ENTITIES> position;
    std::array<Stats, MAX_ENTITIES> stats;
    std::array<Equipment, MAX_ENTITIES> equipment;

    // Lifecycle management
    std::array<int, MAX_ENTITIES> alive_indices;
    std::array<int, MAX_ENTITIES> dead_indices;
    int alive_count = 0;
    int dead_count = MAX_ENTITIES;

    // Track on map
    std::array<std::array<std::optional<size_t>, GRID_HEIGHT>, GRID_WIDTH> grid;
} EntityData;
//////////////////////////
