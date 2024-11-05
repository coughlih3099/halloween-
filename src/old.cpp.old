#include "raylib.h"
#include "raymath.h"


#define TILE_WIDTH 16
#define TILE_HEIGHT 16
#define WORLD_WIDTH 20
#define WORLD_HEIGHT 20
#define MAX_TEXTURES 1


void GameStartup();
void GameUpdate();
void GameRender();
void GameShutdown();
void DrawTile(int pos_x, int pos_y, int texture_index_x, int texture_index_y);

typedef enum {
    TEXTURE_TILEMAP = 0
} texture_asset;

Texture2D textures[MAX_TEXTURES];

typedef enum {
    TILE_TYPE_DIRT,
    TILE_TYPE_GRASS,
    TILE_TYPE_TREE,
    TILE_TYPE_STONE
} tile_type;

typedef struct {
    int x;
    int y;
    int type;
} sTile;

typedef enum {
    ZONE_ALL,
    ZONE_WORLD,
    ZONE_DUNGEON
} eZone;

sTile world[WORLD_WIDTH][WORLD_HEIGHT];
sTile dungeon[WORLD_WIDTH][WORLD_HEIGHT];

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

Camera2D camera = { 0 };

typedef struct {
    int x;
    int y;
    int health;
    int damage;
    eZone zone;
    bool isPassable;
    bool isAlive;
} sEntity;

sEntity player;
sEntity dungeon_gate;
sEntity enemy;

int main() {
    // Init
    // ---

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Window");
    SetTargetFPS(60);

    GameStartup();
    //---

    // Main Loop
    while (!WindowShouldClose()) {
        // Update
        // -----
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
    InitAudioDevice();

    Image image = LoadImage("../assets/colored_packed.png");
    textures[TEXTURE_TILEMAP] = LoadTextureFromImage(image);
    UnloadImage(image);


    for (int w = 0; w < WORLD_WIDTH; w++) {
        for (int h = 0; h < WORLD_HEIGHT; h++) {
            world[w][h] = (sTile) {
                .x = w,
                .y = h,
                .type = GetRandomValue(TILE_TYPE_DIRT, TILE_TYPE_TREE)
            };
            dungeon[w][h] = (sTile) {
                .x = w,
                .y = h,
                .type = TILE_TYPE_DIRT
            };
        }
    }

    // starting position of player
    player = (sEntity) {
        .x = TILE_WIDTH * 3,
        .y = TILE_HEIGHT * 3,
        .health = 100,
        .damage = 0,
        .zone = ZONE_WORLD,
        .isPassable = false,
        .isAlive = true
    };
    // position of dungeon gate
    dungeon_gate = (sEntity) {
        .x = TILE_WIDTH * GetRandomValue(0, WORLD_WIDTH - 1),
        .y = TILE_HEIGHT * GetRandomValue(0, WORLD_HEIGHT - 1),
        .zone = ZONE_ALL,
        .isPassable = true,
    };
    // starting pos of enemy
    enemy = (sEntity) {
        .x = TILE_WIDTH * 10,
        .y = TILE_HEIGHT * 10,
        .health = 100,
        .damage = 0,
        .zone = ZONE_DUNGEON,
        .isPassable = false,
        .isAlive = true
    };


    camera.target = (Vector2){ static_cast<float>(player.x),
                               static_cast<float>(player.y) };
    camera.offset = (Vector2){
        static_cast<float>(SCREEN_WIDTH) / 2,
        static_cast<float>(SCREEN_HEIGHT) / 2
    };
    camera.zoom = 3.0f;
}

void GameUpdate() {
    float x = player.x;
    float y = player.y;
    switch (GetKeyPressed()) {
        case KEY_UP:
            y -= 1 * TILE_HEIGHT;
            break;
        case KEY_LEFT:
            x -= 1 * TILE_WIDTH;
            break;
        case KEY_DOWN:
            y += 1 * TILE_HEIGHT;
            break;
        case KEY_RIGHT:
            x += 1 * TILE_WIDTH;
            break;
    }

    // check for enemy collision
    if (player.zone == enemy.zone &&
        enemy.x == x && enemy.y == y) {
        // roll 2 d10
        int damage = GetRandomValue(1, 10) + GetRandomValue(1, 10);
        enemy.health -= damage;
        enemy.damage = damage;

        if (enemy.health <= 0) {
            enemy.isAlive = false;
        }
    } else {
        player.x = x;
        player.y = y;
        camera.target = (Vector2){ static_cast<float>(player.x),
                                   static_cast<float>(player.y) };
    }


    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        const float ZOOM_INCREMENT = 0.3f;
        camera.zoom += (wheel * ZOOM_INCREMENT);
        if (camera.zoom < 3.0f) {
            camera.zoom = 3.0f;
        }
        if (camera.zoom > 5.0f) {
            camera.zoom = 5.0f;
        }
    }

    if (IsKeyPressed(KEY_E)) {
        if (player.x == dungeon_gate.x && player.y == dungeon_gate.y) {
            if (player.zone == ZONE_WORLD) {
                player.zone = ZONE_DUNGEON;
            } else if (player.zone == ZONE_DUNGEON) {
                player.zone = ZONE_WORLD;
            }
        }
    }
}

void DrawTile(int pos_x, int pos_y, int texture_index_x, int texture_index_y) {
    Rectangle source = {
        static_cast<float>(texture_index_x * TILE_WIDTH),
        static_cast<float>(texture_index_y * TILE_HEIGHT),
        static_cast<float>(TILE_WIDTH),
        static_cast<float>(TILE_HEIGHT)
    };
    Rectangle dest = {
        static_cast<float>(pos_x),
        static_cast<float>(pos_y),
        static_cast<float>(TILE_WIDTH),
        static_cast<float>(TILE_HEIGHT),
    };
    Vector2 origin = { 0, 0 };
    DrawTexturePro(textures[TEXTURE_TILEMAP], source, dest, origin,
                    0.0f, WHITE);
}

void GameRender() {
    BeginMode2D(camera);

    // Draw the map
    sTile tile;
    int texture_index_x;
    int texture_index_y;
    for (int w = 0; w < WORLD_WIDTH; w++) {
        for (int h = 0; h < WORLD_HEIGHT; h++) {
            if (player.zone == ZONE_WORLD) {
                tile = world[w][h];
            } else if (player.zone == ZONE_DUNGEON) {
                tile = dungeon[w][h];
            }
            switch (tile.type) {
                case TILE_TYPE_DIRT:
                    texture_index_x = 2;
                    texture_index_y = 0;
                    break;
                case TILE_TYPE_GRASS:
                    texture_index_x = 5;
                    texture_index_y = 0;
                    break;
                case TILE_TYPE_TREE:
                    texture_index_x = 0;
                    texture_index_y = 1;
                    break;
                case TILE_TYPE_STONE:
                    texture_index_x = 5;
                    texture_index_y = 2;
                    break;
            }
            DrawTile(tile.x * TILE_WIDTH, tile.y * TILE_HEIGHT,
                     texture_index_x, texture_index_y);
        }
    }

    // Draw the dungeon gate
    DrawTile(dungeon_gate.x, dungeon_gate.y, 22, 11);
    // Draw the enemy
    if (enemy.zone == player.zone) {
        if (enemy.isAlive) {
            DrawTile(enemy.x, enemy.y, 26, 2);
        }
    }
    // Draw the player sprite
    DrawTile(camera.target.x, camera.target.y, 26, 0);

    EndMode2D();

    DrawRectangle(5, 5, 330, 240, Fade(SKYBLUE, 0.5f));
    DrawRectangleLines(5, 5, 330, 240, BLUE);

    DrawText(TextFormat("Camera Target: (%06.2f, %06.2f)", camera.target.x,
                        camera.target.y), 7, 7, 18, BLACK);
    DrawText(TextFormat("Camera Zoom: %06.2f", camera.zoom), 7, 26, 18, BLACK);
    DrawText(TextFormat("Player Health: %d", player.health), 7, 45, 18, BLACK);
    DrawText(TextFormat("Enemy Health: %d", enemy.health), 7, 64, 18, BLACK);
}

void GameShutdown() {
    for (int i = 0; i < MAX_TEXTURES; i++) {
        UnloadTexture(textures[i]);
    }

    CloseAudioDevice();
}
