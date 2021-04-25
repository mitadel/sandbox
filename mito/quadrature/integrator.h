#include "../mito.h"
#include "../mesh/element_set.h"
#include "../math/fields.h"
#include "quadrature_rules.h"

namespace mito {

    // TODO: Keep in mind that we will need integrator and the above defined fields to compute
    // integrals of contact forces down the road. Do we have enough machinery for that?

    // template with respect to element type T and to degree of exactness r of quadrature rule
    template <class quadrature_t, class element_t, int r, dim_t D>
    class Integrator {
        // quadrature_t, element_t, and r identify a specific quadrature rule
        using QuadratureRule = QuadratureRulesFactory<quadrature_t, element_t, r>;

      private:
        // QUESTION: Who should be in charge of computing the coordinates of the quadrature points
        //           in the elements? The quadrature rule has the coordinates of the quadrature
        //           points on the reference element, the elements have the coordinate of the
        //           vertices.
        void _computeQuadPointCoordinates()
        {

            // TOFIX: We should avoid the 4 nested for loops
            // QUESTION: 3 out 4 of these loops can be unrolled as Q, D, V are template parameters
            //           Is there anything we can do about it?

            // loop on elements
            int e = 0;
            for (const auto & element : _elementSet.elements()) {
                // use a set to collect vertices without repeated entries
                std::set<const vertex_t *> vertices;
                element->getVertices(vertices);
                // loop on vertices
                int v = 0;
                for (const auto & vertex : vertices) {
                    // loop on quadrature point
                    for (int q = 0; q < Q; ++q) {
                        for (int d = 0; d < D; ++d) {
                            const auto & vertexCoordinates = _elementSet.coordinatesVertex(vertex);
                            _coordinates[e * Q + q][d] +=
                                std::get<0>(_quadratureRule[q])[v] * vertexCoordinates[d];
                        }
                    }
                    ++v;
                }
                ++e;
            }

            // all done
            return;
        }

      public:
        Integrator(const ElementSet<element_t, D> & elementSet) :
            _elementSet(elementSet),
            _coordinates(elementSet.nElements() * Q)
        {
            _computeQuadPointCoordinates();
        }

        real integrate(const ScalarField<D> & function)
        {
            std::cout << "integrating ... " << std::endl;

            std::vector<real> values = function(_coordinates);

            real result = 0.0;

            // TOFIX: Typedef elem_t, quad_t, dim_t so as to give a compilation error if
            // misused
            //        Also: consider using p2::grid to decouple memory from indexing.
            //        Syntax is as follows:
            //              index_t i {e, q, j};
            //              values[i];
            // for (auto & e : _elementSet) {
            for (auto e = 0; e < _elementSet.nElements(); ++e) {
                for (auto q = 0; q < Q; ++q) {
                    result += values[e * Q + q] * std::get<1>(_quadratureRule[q])
                            * _elementSet.jacobian(e);
                }
            }

            return result;
        }

      private:
        // the quadrature rule
        static constexpr auto _quadratureRule = QuadratureRule::Get();
        // the domain of integration
        const ElementSet<element_t, D> & _elementSet;
        // the coordinates of the quadrature points in the domain of integration
        std::vector<mito::vector<D>> _coordinates;
        // the number of element vertices
        static constexpr int V = element_t::nVertices;
        // the number of quadrature points
        static constexpr int Q = _quadratureRule.size();
    };

}    // namespace  mito

// end of file
