// get support for grids
#include <pyre/grid.h>

namespace mito {

    template <typename T, int... I>
    class Grid {

      public:
        static constexpr auto N = sizeof...(I);
        using type = T;

      public:
        // D-dim conventionally packed grid
        using pack_t = pyre::grid::canonical_t<N>;
        // T on the heap
        using storage_t = pyre::memory::heap_t<T>;
        // putting it all together
        using grid_t = pyre::grid::grid_t<pack_t, storage_t>;
        // index typedef
        using index_t = pyre::grid::index_t<N>;

      public:
        // default constructor
        inline Grid() : _packing { { I... } }, _grid { _packing, _packing.cells() } {}

        // delete constructors, operator=
        inline Grid(const Grid &) = delete;
        inline Grid(const Grid &&) = delete;
        inline const Grid & operator=(const Grid &) = delete;
        inline const Grid & operator=(const Grid &&) = delete;

        Grid(const grid_t & grid) : _grid(grid), _packing(grid.layout()) {}
        Grid(grid_t && grid) : _grid(grid), _packing(grid.layout()) {}
        ~Grid() {}

      public:
        inline const T & operator[](index_t i) const { return _grid[i]; }
        inline T & operator[](index_t i) { return _grid[i]; }

        inline void operator+=(const mito::Grid<T, I...> & rhs)
        {
            // loop on components
            for (const auto & idx : _grid.layout()) {
                // component-wise operator+=
                _grid[idx] += rhs[idx];
            }

            // all done
            return;
        }

      public:
        constexpr auto size() { return _packing.cells(); }

      private:
        // packing
        pack_t _packing;
        // grid
        grid_t _grid;
    };

    // TOFIX
    // template <class T = real>
    // using scalar_t = Grid<T, 1>;

    template <int D, class T = real>
    using vector_t = Grid<T, D>;

    // TOFIX
    template <class T = real>
    using scalar_t = vector_t<1>;

    template <int D1, int D2 = D1, class T = real>
    using tensor_t = Grid<T, D1, D2>;

}    // namespace mito

// end of file
