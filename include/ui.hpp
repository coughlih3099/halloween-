#pragma once
#include <raylib.h>
#include "./main.hpp"

namespace UI {
Vector2 get_clamped_camera(Position player_position);
Vector2 get_viewport_offset();
}  // namespace UI
