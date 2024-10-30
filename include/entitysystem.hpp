#pragma once

#include <raylib.h>
#include "./main.hpp"


namespace EntitySystem {
bool is_valid_position(Position pos);
void initialize(EntityData* data);
Position get_new_position(Position current_pos, Direction dir);
bool is_occupied(const EntityData& data, Position current_pos);
bool is_traversable(const EntityData& data, const Tile map[][WORLD_HEIGHT], Position pos);
std::optional<int> create_entity(EntityData* data, const Tile map[][WORLD_HEIGHT], Position Pos);
void kill_entity(EntityData* data, int entity_idx);
bool move_entity(EntityData* data, const Tile map[][WORLD_HEIGHT], int entity_idx, Direction dir);
std::optional<int> get_entity_idx(const EntityData& data, Position pos);
std::optional<int> attack(EntityData* data, int attacker_index, Direction attack_direction);
}  // namespace EntitySystem
