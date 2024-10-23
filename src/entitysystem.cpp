#include <raylib.h>
#include "../include/main.hpp"


/**
 * Checks whether position is within the bounds of the world
 * @param pos, current position to be checked
 */
bool is_valid_position(Position pos) {
    return pos.x >= 0 && pos.x < GRID_WIDTH &&
           pos.y >= 0 && pos.y < GRID_HEIGHT;
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
 * @return new Position location
 */
Position get_new_position(Position current_pos, Direction dir) {
    Position new_pos;
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
 * @param pos, the Position position to check
 */
bool is_occupied(const EntityData& data, Position pos) {
    return is_valid_position(pos) && data.grid[pos.x][pos.y].has_value();
}


/**
 * Check to see if the tile is traversable
 * @param data, EntityStruct reference
 * @param pos, the position to check
 */
bool is_traversable(const EntityData& data, Position pos) {
    return false;
}


/**
 * @param data, EntityData struct pointer
 * @param pos, Position position for entity
 * @return nullopt or entity index
 */
std::optional<size_t> create_entity(EntityData* data, Position pos) {
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


/**
 * Removes the entity from the alive array
 * @param data, EntityData struct pointer
 * @param entity_idx, the index of the entity to remove
 */
void kill_entity(EntityData* data, int entity_idx) {
    for (int i = 0; i < data->alive_count; i++) {
        if (data->alive_indices[i] == entity_idx) {
            // Clear the position of the entity in the grid
            Position pos = data->position[entity_idx];
            data->grid[pos.x][pos.y] = std::nullopt;

            // Remove the entity from the alive array
            data->alive_indices[i] = data->alive_indices[data->alive_count - 1];
            data->alive_count--;

            // Add to the dead array
            data->dead_indices[data->dead_count] = entity_idx;
            data->dead_count++;
        }
    }
}


/**
 * Moves the entity if the position is valid and unoccupied
 * @param data, Entity data struct pointer
 * @param entity_idx, index of the entity to move
 * @param dir, the direction in which to move the entity
 * @return bool, true if the entity moved, false otherwise
 */
bool move_entity(EntityData* data, int entity_idx, Direction dir) {
    bool moved;
    Position current = data->position[entity_idx];
    Position new_pos = get_new_position(current, dir);

    if (!is_valid_position(new_pos) || is_occupied(*data, new_pos)) {
        moved = false;
    } else {
        // update grid
        data->grid[current.x][current.y] = std::nullopt;
        data->grid[new_pos.x][new_pos.y] = entity_idx;

        // update position
        data->position[entity_idx] = new_pos;
        moved = true;
    }
    return moved;
}
