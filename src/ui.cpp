#include <raylib.h>
#include <algorithm>
#include "../include/main.hpp"


namespace UI {

// these numbers came to me through trial and error,
// hopefully they work forever
const float viewport_top_edge = 48.0f;
const float viewport_left_edge = 74.0f;
const float viewport_right_edge = 238.0f;
const float viewport_bottom_edge = 272.0f;

Vector2 get_clamped_camera(Position player_position) {
    Vector2 clamped_position;

    float player_screen_x = player_position.x * TILE_WIDTH;
    float player_screen_y = player_position.y * TILE_HEIGHT;

    clamped_position.x = std::clamp(player_screen_x, viewport_left_edge,
                                    viewport_right_edge);
    clamped_position.y = std::clamp(player_screen_y, viewport_top_edge,
                                    viewport_bottom_edge);

    return clamped_position;
}

}  // namespace UI
