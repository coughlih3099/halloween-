#pragma once

#include <raylib.h>
#include "./main.hpp"


namespace EntitySystem {
bool is_valid_position(Vector2 pos);
void initialize(EntityData* data);
Vector2 get_new_position(Vector2 current_pos, Direction dir);
bool is_occupied(const EntityData& data, Vector2 current_pos);
}
