#include <raylib.h>
#include <algorithm>
#include "../include/main.hpp"


namespace UI {

// these numbers came to me through trial and error,
// hopefully they work forever
const float viewport_top_edge = 48.0f;
const float viewport_left_edge = 74.0f;
const float viewport_right_edge = (WORLD_WIDTH * TILE_WIDTH) - (TILE_HEIGHT * 5) - 1;
const float viewport_bottom_edge = (WORLD_HEIGHT * TILE_HEIGHT) - (TILE_HEIGHT * 3);
//const float viewport_right_edge = 238.0f;
//const float viewport_bottom_edge = 272.0f;

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

Vector2 get_viewport_offset() {
    // I can't remember specifically how I got these values, but what I can say,
    // is for some reason, just search and replace the "edge" part of the
    // variable names to "side" actually breaks the whole thing?
    const float viewport_top_edge = 48.0f;
    const float viewport_left_edge = 320.0f;
    const float viewport_right_edge = 960.0f;
    const float viewport_bottom_edge = 480.0f;
    const float viewport_width_middle = (viewport_right_edge - viewport_left_edge) / 2;
    const float viewport_length_middle = (viewport_bottom_edge - viewport_top_edge) / 2;
    const Vector2 VIEWPORT_MIDDLE = { viewport_width_middle + viewport_left_edge,
                                    viewport_top_edge  + viewport_length_middle };
    return VIEWPORT_MIDDLE;
}

}  // namespace UI
