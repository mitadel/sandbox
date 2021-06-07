// get support for grids
#include <pyre/grid.h>

namespace mito {

    template <typename... Args>
    constexpr auto multiply(Args &&... args)
    {
        return (args * ...);
    }

    template <typename T, int... I>
    class Grid {

      public:
        // compute the number of degrees of freedom of the
        static constexpr auto N = sizeof...(I);
        // compute the number of cells of the packing
        static constexpr auto S = multiply(I...);
        using type = T;

      public:
        // N-dim conventionally packed grid
        using pack_t = pyre::grid::canonical_t<N>;
        // T on the heap
        using storage_t = pyre::memory::heap_t<T>;
        // putting it all together
        using grid_t = pyre::grid::grid_t<pack_t, storage_t>;
        // index typedef
        using index_t = pyre::grid::index_t<N>;

      public:
        // default constructor
        inline Grid() : _packing { { I... } }, _grid { _packing, S }
        {
            // initialize memory
            initialize();

            // all done
            return;
        }

        // TOFIX: enable if
        //      1) can cast T2 to T, and
        //      2) sizeof...(T2) == S
        template <class... T2 /*, typename std::enable_if<sizeof...(T2) == S, int>::type = 0*/>
        Grid(T2... args) : Grid()
        {
            _initialize(std::make_index_sequence<S> {}, args...);

            // all done
            return;
        }

        // delete constructors, operator=
        // TOFIX: double check if default implementation will do
        Grid(const Grid &) = default;
        Grid(const Grid &&) = delete;
        const Grid & operator=(const Grid &) = delete;
        const Grid & operator=(const Grid &&) = delete;

        inline Grid(const grid_t & grid) : _grid(grid), _packing(grid.layout()) {}
        inline Grid(grid_t && grid) : _grid(grid), _packing(grid.layout()) {}
        inline ~Grid() {}

      public:
        inline const T & operator[](index_t i) const { return _grid[i]; }
        inline T & operator[](index_t i) { return _grid[i]; }

        inline const T & operator[](int i) const { return _grid[i]; }
        inline T & operator[](int i) { return _grid[i]; }

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

        /**
         * reset all entries to zero
         */
        inline void initialize()
        {
            for (const auto & idx : _grid.layout()) {
                _grid[idx] = T(0);
            }

            return;
        }

        template <size_t... J, class... T2>
        void _initialize(std::index_sequence<J...>, T2... args)
        {
            ((_grid[J] = args), ...);
        }

      public:
        constexpr auto size() { return S; }

      private:
        // packing
        pack_t _packing;
        // grid
        grid_t _grid;
    };

    // template specialization for scalars
    template <typename T>
    class Grid<T, 1> {
      public:
        // compute the number of degrees of freedom of the
        static constexpr auto N = 1;
        // compute the number of cells of the packing
        static constexpr auto S = 1;
        using type = T;

      public:
        // N-dim conventionally packed grid
        using pack_t = pyre::grid::canonical_t<N>;
        // T on the heap
        using storage_t = pyre::memory::heap_t<T>;
        // putting it all together
        using grid_t = pyre::grid::grid_t<pack_t, storage_t>;
        // index typedef
        using index_t = pyre::grid::index_t<N>;

      public:
        // default constructor
        inline Grid() : _packing { { 1 } }, _grid { _packing, S }
        {
            assert(S == _packing.cells());

            // initialize memory
            _grid[0] = T(0);

            // all done
            return;
        }

        Grid(T arg) : Grid()
        {
            _grid[0] = arg;

            // all done
            return;
        }

        // delete constructors, operator=
        // TOFIX: double check if default implementation will do
        inline Grid(const Grid &) = default;
        inline Grid(const Grid &&) = delete;
        inline const Grid & operator=(const Grid &) = delete;
        inline const Grid & operator=(const Grid &&) = delete;

        Grid(const grid_t & grid) : _grid(grid), _packing(grid.layout()) {}
        Grid(grid_t && grid) : _grid(grid), _packing(grid.layout()) {}
        ~Grid() {}

        operator T() const { return _grid[0]; }

      public:
        inline const T & operator[](index_t i) const { return _grid[i]; }
        inline T & operator[](index_t i) { return _grid[i]; }

        inline const T & operator[](int i) const { return _grid[i]; }
        inline T & operator[](int i) { return _grid[i]; }

        inline void operator+=(const mito::Grid<T, 1> & rhs)
        {
            // component-wise operator+=
            _grid[0] += rhs[0];

            // all done
            return;
        }

      public:
        constexpr auto size() { return S; }

      private:
        // packing
        pack_t _packing;
        // grid
        grid_t _grid;
    };

}    // namespace mito

// end of file
