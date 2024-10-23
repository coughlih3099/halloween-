#pragma once

#include <raylib.h>
#include "./main.hpp"


namespace EntitySystem {
bool is_valid_position(Position pos);
void initialize(EntityData* data);
Position get_new_position(Position current_pos, Direction dir);
bool is_occupied(const EntityData& data, Position current_pos);
}
