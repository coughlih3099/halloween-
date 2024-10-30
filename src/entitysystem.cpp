#include <raylib.h>
#include "../include/main.hpp"

namespace EntitySystem {

/**
 * Initialize the arrays
 */
void initialize(EntityData* data) {
    for (int i = 0; i < MAX_ENTITIES; i++) {
        data->dead_indices[i] = i;
        data->positions[i] = { 0, 0 };
        data->stats[i] = std::nullopt;
        // data->equipment[i] = std::nullopt;
    }
    // Clear the grid
    for (auto& column : data->grid) {
        column.fill(std::nullopt);
    }
}


/**
 * Checks whether position is within the bounds of the world
 * @param position, current position to be checked
 */
bool is_valid_position(Position position) {
    return position.x >= 0 && position.x < GRID_WIDTH &&
           position.y >= 0 && position.y < GRID_HEIGHT;
}


/**
 * Check to see if the tile position has an occupant
 * @param data, EntityData struct
 * @param position, the Position position to check
 */
bool is_occupied(const EntityData& data, Position position) {
    return is_valid_position(position) && data.grid[position.x][position.y].has_value();
}


/**
 * calls is_valid_position and is_occupied to check if the position is valid
 * within the data.grid, then looks through the tile map to check to see if
 * the tile is traversable
 * @param data, EntityStruct reference
 * @param map, the map data to check
 * @param position, the position to check
 * @return true if position is traversable, default return is false
 */
bool is_traversable(const EntityData& data, const Tile map[][WORLD_HEIGHT], Position position) {
    if (is_valid_position(position) && !is_occupied(data, position)) {
        const Tile& tile = map[position.x][position.y];
            return tile.traversable;
    }
    return false;
}


/**
 * Returns a new position based on the current position and direction to move
 * @param current_pos, entity's current position
 * @param direction, direction enum value to move the entity in
 * @return new Position location
 */
Position get_new_position(Position current_pos, Direction direction) {
    Position new_pos;
    switch (direction) {
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
 * @param data, EntityData struct pointer
 * @param position, Position position for entity
 * @return nullopt or entity index
 */
std::optional<int> create_entity(EntityData* data, const Tile map[][WORLD_HEIGHT], Position position) {
    TraceLog(LOG_INFO, "Attempting to create entity at position (%d, %d)",
             position.x, position.y);

    if (data->dead_count == 0 || !(is_traversable(*data, map, position))) {
        return std::nullopt;
    }

    // get index from available indices and decrement amount left
    int entity_index = data->dead_indices[data->dead_count - 1];
    data->dead_count--;

    // add the new entity to the alive array and increment alive count
    data->alive_indices[data->alive_count] = entity_index;
    data->alive_count++;

    // update the entity position and add them to the grid
    data->positions[entity_index] = position;
    data->grid[position.x][position.y] = entity_index;

    TraceLog(LOG_INFO, "successfully created entity with index %d", entity_index);
    return entity_index;
}


/**
 * Removes the entity from the alive array
 * @param data, EntityData struct pointer
 * @param entity_index, the index of the entity to remove
 */
void kill_entity(EntityData* data, int entity_index) {
    for (int i = 0; i < data->alive_count; i++) {
        if (data->alive_indices[i] == entity_index) {
            // Clear the position of the entity in the grid
            Position position = data->positions[entity_index];
            data->grid[position.x][position.y] = std::nullopt;

            // Remove the entity from the alive array
            data->alive_indices[i] = data->alive_indices[data->alive_count - 1];
            data->alive_count--;

            // Add to the dead array
            data->dead_indices[data->dead_count] = entity_index;
            data->dead_count++;
        }
    }
}


/**
 * Moves the entity if the position is valid and unoccupied
 * @param data, Entity data struct pointer
 * @param map, reference to 2d array of map tiles
 * @param entity_index, index of the entity to move
 * @param direction, the direction in which to move the entity
 * @return bool, true if the entity moved, false otherwise
 */
bool move_entity(EntityData* data, const Tile map[][WORLD_HEIGHT], int entity_index, Direction direction) {
    bool moved;
    Position current = data->positions[entity_index];
    Position new_pos = get_new_position(current, direction);

    if (is_traversable(*data, map, new_pos)) {
        // update grid
        data->grid[current.x][current.y] = std::nullopt;
        data->grid[new_pos.x][new_pos.y] = entity_index;

        // update position
        data->positions[entity_index] = new_pos;
        moved = true;
    } else {
        moved = false;
    }
    return moved;
}

/**
 * Gets the entity index for the for the specified position within the grid
 * @param data, entity data struct reference
 * @param position, the position of the entity to return the index for
 * @return entity index or nullopt
 */
std::optional<int> get_entity_index(const EntityData& data, Position position) {
    return data.grid[position.x][position.y];
}

/**
 * Attack an entity
 * @param data, entity data struct pointer
 * @param attacker_index, the index of the entity doing the attacking
 * @param attack_direction, the direction in which the attacker is attacking
 * @return damage done by the entity or nullopt
 */
std::optional<int> attack(EntityData* data, int attacker_index, Direction attack_direction) {
    std::optional<int> damage_done = std::nullopt;
    Position attacker_position = data->positions[attacker_index];
    Position defender_position = get_new_position(
        attacker_position, attack_direction);
    std::optional<int> defender_index = get_entity_index(*data, defender_position);
    // Check if the defender exists and has a health stat
    if (defender_index.has_value() &&
        data->stats[defender_index.value()]->attributes["Health"] > 0) {
    TraceLog(LOG_INFO, "Entity: %d at position: %d, %d made an attack against entity: %d at position %d, %d",
             attacker_index, attacker_position.x, attacker_position.y,
             defender_index, defender_position.x, defender_position.y);
        // TODO(coughlih3099): Make a more complex damage calculation
        // do damage calculation
        damage_done = data->stats[attacker_index]->attributes["Attack"];
        data->stats[defender_index.value()]->attributes["Health"] -= damage_done.value();
    }
    return damage_done;
}

}  // namespace EntitySystem
