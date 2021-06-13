#include <iostream>
// support
#include <cassert>
// get the grid
#include <pyre/grid.h>

#include "../../mito.h"

namespace mito {

    template <int D, typename T = real>
    class Vector {

      public:
        // 1d conventionally packed grid
        using pack_t = pyre::grid::canonical_t<1>;
        // T on the heap
        using storage_t = pyre::memory::heap_t<T>;
        // putting it all together
        using grid_t = pyre::grid::grid_t<pack_t, storage_t>;

        inline Vector() : Vector(pack_t { { D } }) {}

      private:
        inline Vector(const pack_t & packing) : _grid { packing, packing.cells() } {}

      public:
        // inline Vector(const Vector &) = delete;
        // inline Vector(const Vector &&) = delete;
        // inline const Vector & operator=(const Vector &) = delete;
        // inline const Vector & operator=(const Vector &&) = delete;

        Vector(const grid_t & grid) : _grid(grid) {}
        Vector(grid_t && grid) : _grid(grid) {}
        ~Vector() {}

        static constexpr int size() { return D; }
        using type = T;

        inline const T & operator[](int i) const { return _grid[{ i }]; }
        inline T & operator[](int i) { return _grid[{ i }]; }

      private:
        // grid
        grid_t _grid;
    };

    template <int Q, typename Y>
    class QuadratureFieldGrid {

        static constexpr int D = Y::size();
        using T = typename Y::type;

        // conventionally packed grid for {e, q, d}
        using pack_t = pyre::grid::canonical_t<3>;
        // of Y on the heap
        using storage_t = pyre::memory::heap_t<T>;
        // putting it all together
        using grid_t = pyre::grid::grid_t<pack_t, storage_t>;

      public:
        /**
         * constructor
         * @param[in] elements number of elements for which data are stored
         */
        inline QuadratureFieldGrid(int nElements) :
            QuadratureFieldGrid(nElements, pack_t { { nElements, Q, D } })
        {}

      private:
        inline QuadratureFieldGrid(int nElements, const pack_t & packing) :
            _grid { packing, packing.cells() }
        {}

      public:
        // destructor
        ~QuadratureFieldGrid() {}

      public:
        /**
         * mutator to an array of data stored at a quadrature point of an element
         * @param[in] e index of the element
         * @param[in] q local index of the quadrature point in the element
         * @return a pointer to the data
         */
        // QUESTION: how do we make the const version of this???
        inline Y operator()(int e, int q)
        {
            // slices at {e, q}
            pack_t::index_type index { e, q, 0 };

            // shape dictated by D (Y::size())
            pack_t::shape_type shape { 0, 0, D };

            // slice the grid
            auto sliced_grid = _grid.template slice<1>(index, shape);

            //
            return Y(sliced_grid);
        }

        // TOFIX: This method promises to be const but is not really const, as the returned object
        //        is able to change the memory content of this quadrature field
        inline const Y operator()(int e, int q) const
        {
            // slices at {e, q}
            pack_t::index_type index { e, q, 0 };

            // shape dictated by D (Y::size())
            pack_t::shape_type shape { 0, 0, D };

            // slice the grid
            auto sliced_grid = _grid.template slice<1>(index, shape);

            //
            return Y(sliced_grid);
        }

        /**
         * accessor for the size of array stored per quadrature point per element
         * @return the size of array stored per quadrature point per element
         */
        static constexpr int n_dim() { return D; }

        /**
         * accessor for the number of elements
         * @return the number of elements
         */
        inline int n_elements() const { return _grid.layout().shape()[0]; }

        /*
         * accessor for the quadrature points per element
         * @return the number of quadrature point per element
         */
        static constexpr int n_quad() { return Q; }

      private:
        // instantiate the grid
        grid_t _grid;
    };
}

template <int Q, typename Y>
void
myConstFunction(const mito::QuadratureFieldGrid<Q, Y> & quadF)
{
    mito::Vector<2> myvector = quadF(0, 1);
    myvector[1] = 10;
    return;
}

// verify the basic layout of memory mapped grids
int
main(int argc, char * argv[])
{
    // we'll work with a 3d conventionally packed grid
    using pack_t = pyre::grid::canonical_t<3>;
    // of mito::vector<2> on the heap
    using storage_t = pyre::memory::heap_t<mito::real>;
    // putting it all together
    using grid_t = pyre::grid::grid_t<pack_t, storage_t>;

    // packing: 1024x1024x8
    pack_t packing { { 1024, 1024, 8 } };
    // instantiate the grid
    grid_t grid { packing, packing.cells() };

    // tryout slices
    pack_t::index_type index { 0, 0, 1 };
    // QUESTION: What if shape {8, 0, 0} and slice<2> ?
    pack_t::shape_type shape { 0, 0, 8 };

    // slice of rank 1 (directly grid)
    auto sliced_grid = grid.slice<1>(index, shape);
    auto sliced_grid_a = grid.slice<2>(index, shape);
    // verify that the address is the same regardless of the way it is computed
    assert((&sliced_grid[{ 1 }] == &sliced_grid_a[{ 0, 1 }]));

    int nElements = 20;
    constexpr int Q = 10;
    mito::QuadratureFieldGrid<Q, mito::Vector<2>> quadratureField(nElements);

    mito::Vector<2> myvector = quadratureField(0, 1);
    myvector[0] = 0.0;
    myvector[1] = 1.0;

    mito::Vector<2> myvector2 = quadratureField(0, 1);
    std::cout << myvector2[0] << "\t" << myvector2[1] << std::endl;

    myConstFunction(quadratureField);

    // all done
    return 0;
}