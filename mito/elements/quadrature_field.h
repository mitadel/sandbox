#include "../mito.h"

#ifndef __MITO__QUADRATURE_FIELD__
#define __MITO__QUADRATURE_FIELD__

namespace mito {
    template <int Q, typename Y>
    class QuadratureField {

      public:
        static constexpr int D = Y::size();
        using T = typename Y::type;

      private:
        // conventionally packed grid for {e, q, d}
        using pack_t = pyre::grid::canonical_t<3>;
        // of T on the heap
        using storage_t = pyre::memory::heap_t<T>;
        // putting it all together
        using grid_t = pyre::grid::grid_t<pack_t, storage_t>;

      public:
        /**
         * constructor
         * @param[in] elements number of elements for which data are stored
         */
        inline QuadratureField(int nElements) :
            _packing { { nElements, Q, D } },
            _grid { _packing, _packing.cells() }
        {
            // initialize memory
            initialize();

            // all done
            return;
        }

        // destructor
        ~QuadratureField() {}

      public:
        /**
         * mutator to an array of data stored at a quadrature point of an element
         * @param[in] e index of the element
         * @param[in] q local index of the quadrature point in the element
         * @return the data
         */
        inline Y operator()(int e, int q)
        {
            // slices at {e, q}
            pack_t::index_type index { e, q, 0 };

            // shape dictated by D (Y::size())
            pack_t::shape_type shape { 0, 0, D };

            // TODO: activate isConst template parameter once it is implemented in pyre
            // slice the grid
            auto sliced_grid =
                _grid.template slice</* sliceRank = */ 1 /*, */ /*isConst = */ /* false */>(
                    index, shape);

            //
            return Y(sliced_grid);
        }

        inline Y operator()(int e, int q) const
        {
            // slices at {e, q}
            pack_t::index_type index { e, q, 0 };

            // shape dictated by D (Y::size())
            pack_t::shape_type shape { 0, 0, D };

            // TODO: activate isConst template parameter once it is implemented in pyre
            // slice the grid
            auto sliced_grid =
                _grid.template slice</* sliceRank = */ 1 /*, */ /*isConst = */ /* true */>(
                    index, shape);

            //
            return Y(sliced_grid);
        }

        /**
         * accessor for the size of array stored per quadrature point per element
         * @return the size of array stored per quadrature point per element
         */
        inline constexpr int n_dim() { return D; }

        /**
         * accessor for the number of elements
         * @return the number of elements
         */
        inline int n_elements() const { return _nElements; }

        /*
         * accessor for the quadrature points per element
         * @return the number of quadrature point per element
         */
        inline constexpr int n_quad() { return Q; }

        /**
         * reset all entries to zero
         */
        inline void initialize()
        {
            for (const auto & idx : _grid.layout()) {
                _grid[idx] = 0.0;
            }

            return;
        }

        /**
         * accessor for name
         */
        inline std::string name() const { return _name; }

      private:
        // number of elements
        int _nElements;

        // packing
        pack_t _packing;
        // instantiate the grid
        grid_t _grid;

        // name of the field
        std::string _name;
    };

    template <int Q, typename Y>
    using quadrature_field_t = QuadratureField<Q, Y>;

}    // namespace mito

#endif    //__MITO__QUADRATURE_FIELD__

// end of file
