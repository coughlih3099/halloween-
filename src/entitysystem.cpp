#include <raylib.h>
#include "../include/main.hpp"


/**
 * Checks whether position is within the bounds of the world
 * @param pos, current position to be checked
 */
bool is_valid_position(Vector2 pos) {
    return pos.x >= 0 && pos.x < WORLD_WIDTH &&
           pos.y >= 0 && pos.y < WORLD_HEIGHT;
}


/**
 * Initialize the arrays
 */
void initialize(EntityData* data) {
    for (size_t i = 0; i < MAX_ENTITIES; i++) {
        data->dead_indices[i] = i;
        data->position[i] = { 0, 0 };
    }

    // Clear the grid
    for (auto& column : data->grid) {
        column.fill(std::nullopt);
    }
}


/**
 * Returns a new position based on the current position and direction to move
 * @param current_pos, entity's current position as a vector2
 * @param dir, direction enum value to move the entity in
 * @return new Vector2 location
 */
Vector2 get_new_position(Vector2 current_pos, Direction dir) {
    Vector2 new_pos;
    switch (dir) {
        case NORTH:
            new_pos = { current_pos.x, current_pos.y - 1 };
            break;
        case EAST:
            new_pos = { current_pos.x - 1, current_pos.y };
            break;
        case SOUTH:
            new_pos = { current_pos.x, current_pos.y + 1 };
            break;
        case WEST:
            new_pos = { current_pos.x + 1, current_pos.y };
            break;
        default:
            new_pos = current_pos;
    }
    return new_pos;
}


/**
 * Check to see if the tile pos has an occupant
 * @param data, EntityData struct
 * @param pos, the Vector2 position to check
 */
bool is_occupied(const EntityData& data, Vector2 pos) {
    return is_valid_position(pos) && data.grid[pos.x][pos.y].has_value();
}


/**
 * @param data, EntityData struct pointer
 * @param pos, Vector2 position for entity
 * @return nullopt or entity index
 */
std::optional<size_t> create_entity(EntityData* data, Vector2 pos) {
    if (!is_valid_position(pos) || is_occupied(*data, pos)
        || data->dead_count ==0) {
        return std::nullopt;
    }

    // get idx from available indices and decrement amount left
    size_t entity_idx = data->dead_indices[data->dead_count - 1];
    data->dead_count--;

    // add the new entity to the alive array and increment alive count
    data->alive_indices[data->alive_count] = entity_idx;
    data->alive_count++;

    // update the entity position and add them to the grid
    data->position[entity_idx] = pos;
    data->grid[pos.x][pos.y] = entity_idx;

    return entity_idx;
}
