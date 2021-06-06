#include "../../mito.h"
#include "../../math/algebra.h"

int
main()
{
    using grid_t = mito::Grid<mito::real, 1, 2, 3, 4>;
    grid_t grid;

    constexpr grid_t::index_t index { 0, 0, 0, 0 };

    grid[index] = 10.0;
    std::cout << "grid = " << grid[index] << std::endl;
    assert(grid[index] == 10.0);

    mito::real a = grid[index];
    a = 11.0;
    std::cout << "grid = " << grid[index] << std::endl;
    assert(grid[index] == 10.0);

    mito::real & b = grid[index];
    b = 11.0;
    std::cout << "grid = " << grid[index] << std::endl;
    assert(grid[index] == 11.0);
    assert((grid[{ 0, 0, 0, 1 }] == 0.0));

    grid += grid;
    assert(grid[index] == 22.0);

    assert(grid.size() == 24);

    mito::scalar_t scalar;
    assert(scalar.size() == 1);

    mito::vector_t<2> vector;
    assert(vector.size() == 2);

    mito::tensor_t<2> tensor;
    assert(tensor.size() == 4);

    return 0;
}