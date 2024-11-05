#pragma once

#include <iostream>
#include <vector>
#include <random>
#include <cassert>

namespace CellularAutomaton {

typedef struct Cell {
    enum Type {
        FLOOR,
        WALL,
    } Type;
} Cell;

typedef struct Grid {
    int width;
    int height;
    std::vector<std::vector<Cell>> cells;
} Grid;

/**
 * Creates and initializes a new Grid with the specified dimensions
 * @param width, the width of the grid, positive integer greater than zero
 * @param height, the height of the grid, positive integer greater than zero
 * @return, initialized Grid with all the cells set to floor
 */
Grid create_grid(int height, int width);

/**
 * Fill the grid with a percentage of floor/wall tiles. Lower density means
 * more floor cells and higher density means more wall cells. Because of the
 * random nature, the percentage may not always be exact
 * @param noise_grid, pointer to noise_grid struct
 * @param density, the density of noise as a percentage of 100, integer between 0 and 100
 * @param seed, optional seed for random number generation
 */
void fill_noise_grid(Grid* noise_grid, int density, unsigned int seed = std::random_device {}());

/**
 * Run n number of iterations through the grid to evolve it
 * @param noise_grid, pointer to noise_grid struct
 * @param iteration, number of iterations to run
 * @param interactive, if true, ask to continue after each iteration, else run through all iterations
 */
void evolve_grid(Grid* grid, unsigned int iterations);

}  // namespace CellularAutomaton
