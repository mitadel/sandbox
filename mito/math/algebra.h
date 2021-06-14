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
        static constexpr int N = sizeof...(I);
        // compute the number of cells of the packing
        static constexpr int S = multiply(I...);
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
        inline Grid() : _grid { pack_t { { I... } }, S }
        {
            // std::cout << "Default Constructed " << std::endl;

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

        // copy constructor (deep copy)
        Grid(const Grid & grid) : _grid { pack_t { { I... } }, S }
        {
            // std::cout << "copy Constructed " << std::endl;
            // component-wise assignment
            for (int i = 0; i < S; ++i) {
                _grid[i] = grid[i];
            }

            // all done
            return;
        }

        // move constructor (shallow copy)
        Grid(Grid && grid) = default;

        // copy assignment operator
        Grid & operator=(const Grid & rhs)
        {
            // assert dimensions are compatible
            assert(this->layout().cells() == rhs.layout().cells());

            // component-wise assignment
            for (int i = 0; i < _grid.layout().cells(); ++i) {
                _grid[i] = rhs[i];
            }

            // QUESTION: Why does this not work? lhs is not changed outside this scope
            // component-wise assignment
            // for (const auto & idx : _grid.layout()) {
            //    _grid[idx] = rhs[idx];
            //}

            // all done
            return *this;
        }

        // move assignment operator
        Grid & operator=(Grid && rhs)
        {
            // assert dimensions are compatible
            assert(this->layout().cells() == rhs.layout().cells());

            // component-wise assignment
            for (int i = 0; i < _grid.layout().cells(); ++i) {
                _grid[i] = rhs[i];
            }

            // all done
            return *this;
        }

        inline Grid(const grid_t & grid) : _grid(grid) {}
        inline Grid(grid_t && grid) : _grid(grid) {}
        inline ~Grid()
        { /*std::cout << "Destructor called" << std::endl;*/
        }

      public:
        inline const T & operator[](index_t i) const { return _grid[i]; }
        inline T & operator[](index_t i) { return _grid[i]; }

        inline const T & operator[](int i) const { return _grid[i]; }
        inline T & operator[](int i) { return _grid[i]; }

        inline void operator+=(const mito::Grid<T, I...> & rhs)
        {
            // loop on components
            for (int i = 0; i < _grid.layout().cells(); ++i) {
                // component-wise operator+=
                _grid[i] += rhs[i];
            }

            // all done
            return;
        }

        inline auto layout() const { return _grid.layout(); }

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
        // grid
        grid_t _grid;
    };

    template <typename T, int... I>
    inline bool operator==(const Grid<T, I...> & lhs, const Grid<T, I...> & rhs)
    {
        // shapes are identical by construction, so compare components
        for (int i = 0; i < lhs.layout().cells(); ++i) {
            // component-wise comparison
            if (lhs[i] != rhs[i]) {
                return false;
            }
        }

        // all done
        return true;
    }

    // template specialization for scalars
    template <typename T>
    class Grid<T, 1> {
      public:
        // compute the number of degrees of freedom of the
        static constexpr int N = 1;
        // compute the number of cells of the packing
        static constexpr int S = 1;
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
        inline Grid() : _grid { pack_t { { 1 } }, S }
        {
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

        // copy constructor (deep copy)
        Grid(const Grid & grid) : _grid { pack_t { { 1 } }, S }
        {
            _grid[0] = grid[0];

            // all done
            return;
        }

        // move constructor (shallow copy)
        inline Grid(Grid &&) = default;

        // copy assignment operator
        Grid & operator=(const Grid & rhs)
        {
            // component-wise assignment
            _grid[0] = rhs[0];

            // all done
            return *this;
        }

        // move assignment operator
        Grid & operator=(const Grid && rhs)
        {
            // component-wise assignment
            _grid[0] = rhs[0];

            // all done
            return *this;
        }

        Grid(const grid_t & grid) : _grid(grid) {}
        Grid(grid_t && grid) : _grid(grid) {}
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
        // grid
        grid_t _grid;
    };

    template <typename T>
    inline bool operator==(const Grid<T, 1> & lhs, const Grid<T, 1> & rhs)
    {
        return lhs[0] == rhs[0];
    }

}    // namespace mito

// end of file
