#include "../include/cellularautomaton.hpp"

#include <iostream>
#include <random>
#include <vector>
#include <cassert>
#include "../include/getinputcpp.hpp"

namespace CellularAutomaton {

Grid create_grid(int height, int width) {
    assert(width > 0 && "Grid width must be positive");
    assert(height > 0 && "Grid height must be positive");

    Grid grid;
    grid.height = height;
    grid.width = width;
    // Cells will be automatically initialized to FLOOR
    grid.cells.resize(height, std::vector<Cell>(width));

    assert(grid.width == width && "Grid width not initialized correctly");
    assert(grid.height == height && "Grid height not initialized correctly");
    assert(grid.cells.size() == height && "Grid rows not initialized to height");
    assert(grid.cells[0].size() == width && "Grid columns not initialized to width");
    return grid;
}

void fill_noise_grid(Grid* noise_grid, int density, unsigned int seed) {
    assert(noise_grid != nullptr && "Null grid pointer");
    assert(density >= 0 && density <= 100 && "Density must be between 0 and 100");
    assert(!noise_grid->cells.empty() && "Grid has no rows");
    assert(!noise_grid->cells[0].empty() && "Grid has no columns");
    assert(seed >= 0 && "Seed value doesn't exist");

    std::mt19937 gen(seed);
    std::uniform_int_distribution<> dist(0, 99);
    int random_val;

    for (auto& row : noise_grid->cells) {
        for (auto& cell : row) {
            random_val = dist(gen);
            // wall is 0 < density, floor is >= density + 1
            cell.Type = (random_val < density) ? Cell::WALL : Cell::FLOOR;
        }
    }
}

void evolve_grid(Grid* grid, unsigned int iterations) {
    assert(grid != nullptr && "Null grid pointer");
    assert(iterations >= 0 && "Iterations less than zero");
    unsigned int passes = 0;
    int choice;
    int neighboring_walls;

    // run at least one pass if interactive, then get input
    while (passes < iterations) {
        Grid temp = *grid;
        for (int i = 0; i < grid->height; i++) {
            for (int j = 0; j < grid->width; j++) {
                neighboring_walls = 0;
                // Check surrounding cells
                for (int y = i - 1; y <= i + 1; y++) {
                    for (int x = j - 1; x <= j + 1; x++) {
                        // Skip the cell we're checking around
                        if (y == i && x == j) {
                            continue;
                        }
                        // If out of bounds or a wall, increment wall count
                        if (y < 0 || x < 0 ||
                            y >= grid->height || x >= grid->width) {
                            neighboring_walls++;
                        } else if (temp.cells[y][x].Type == Cell::WALL) {
                            neighboring_walls++;
                        }
                    }
                }

                // If the cell has 4 or more wall neighbors, become a wall
                if (temp.cells[i][j].Type == Cell::WALL) {
                    grid->cells[i][j].Type = (neighboring_walls >= 4 &&
                                              neighboring_walls <= 5) ?
                                              Cell::WALL : Cell::FLOOR;
                } else {
                    grid->cells[i][j].Type = (neighboring_walls >= 5) ?
                                              Cell::WALL : Cell::FLOOR;
                }
            }
        }
        passes++;
    }
}

}  // namespace CellularAutomaton
