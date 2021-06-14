#ifndef __MITO__ELEMENT_SET__
#define __MITO__ELEMENT_SET__

#include "simplex.h"
#include "vertex_coordinates_map.h"

namespace mito {

    template <int D>
    real computeDistance(const point_t<D> & pointA, const point_t<D> & pointB)
    {
        // return the distance between the two points
        real dist2 = 0.0;
        for (int d = 0; d < D; ++d) {
            real dist_d = pointA[d] - pointB[d];
            dist2 += dist_d * dist_d;
        }

        return sqrt(dist2);
    }

    template <int D>
    void computeSimplicesVolume(
        const std::vector<Simplex<D> *> & elements, const VertexCoordinatesMap<D> & coordinatesMap,
        std::vector<real> & volumes)
    {
        // number of vertices
        constexpr int V = int(D) + 1;

        // a container to store the coordinates of each vertex in a tensor
        static tensor<V> verticesTensor;

        // assert memory allocation is consistent
        assert(volumes.size() == elements.size());

        // loop on elements
        int e = 0;
        for (const auto & element : elements) {

            // reinitialize verticesTensor for a new element
            verticesTensor.initialize();

            // use a set to collect vertices without repeated entries
            std::set<const vertex_t *> vertices;
            element->getVertices(vertices);
            // assert you found D+1 vertices
            assert(V == vertices.size());

            // loop on vertices
            int v = 0;
            for (const auto & vertex : vertices) {
                // fill up verticesTensor container
                for (int d = 0; d < D; ++d) {
                    verticesTensor[v * V + d] = coordinatesMap[vertex][d];
                }
                verticesTensor[v * V + D] = 1.0;
                // update vertices counter
                ++v;
            }

            // compute the volume of the e-th element
            volumes[e] = fabs(ComputeDeterminant(verticesTensor)) / Factorial<D>();
            // update elements counter
            ++e;
        }

        // all done
        return;
    }

    template <class element_t, int D>
    void computeElementsVolume(
        const std::vector<element_t *> & elements, const VertexCoordinatesMap<D> & coordinatesMap,
        std::vector<real> & volumes);

    template <>
    void computeElementsVolume<triangle_t, 2>(
        const std::vector<triangle_t *> & elements, const VertexCoordinatesMap<2> & coordinatesMap,
        std::vector<real> & volumes)
    {
        return computeSimplicesVolume<2>(elements, coordinatesMap, volumes);
    }

    template <>
    void computeElementsVolume<tetrahedron_t, 3>(
        const std::vector<tetrahedron_t *> & elements,
        const VertexCoordinatesMap<3> & coordinatesMap, std::vector<real> & volumes)
    {
        return computeSimplicesVolume<3>(elements, coordinatesMap, volumes);
    }

    template <>
    void computeElementsVolume<segment_t, 1>(
        const std::vector<segment_t *> & elements, const VertexCoordinatesMap<1> & coordinatesMap,
        std::vector<real> & volumes)
    {
        return computeSimplicesVolume<1>(elements, coordinatesMap, volumes);
    }

    template <int D>
    void computeSegmentsLength(
        const std::vector<segment_t *> & elements, const VertexCoordinatesMap<D> & coordinatesMap,
        std::vector<real> & length)
    {
        // number of vertices
        constexpr int V = 2;

        // assert memory allocation is consistent
        assert(length.size() == elements.size());

        // loop on elements
        int e = 0;
        for (const auto & element : elements) {

            // collect vertices
            std::set<const vertex_t *> vertices_set;
            element->getVertices(vertices_set);
            std::vector<const vertex_t *> vertices(vertices_set.begin(), vertices_set.end());

            // assert the size of vertices container is equal to the number of vertices
            assert(vertices.size() == V);

            // store the distance between the two vertices as the element length
            length[e] =
                computeDistance<D>(coordinatesMap[vertices[0]], coordinatesMap[vertices[1]]);

            // update elements counter
            ++e;
        }

        // all done
        return;
    }

    template <>
    void computeElementsVolume<segment_t, 2>(
        const std::vector<segment_t *> & elements, const VertexCoordinatesMap<2> & coordinatesMap,
        std::vector<real> & volumes)
    {
        return computeSegmentsLength<2>(elements, coordinatesMap, volumes);
    }

    template <>
    void computeElementsVolume<segment_t, 3>(
        const std::vector<segment_t *> & elements, const VertexCoordinatesMap<3> & coordinatesMap,
        std::vector<real> & volumes)
    {
        return computeSegmentsLength<3>(elements, coordinatesMap, volumes);
    }

    // follows implementation by Kahan2014
    template <int D = 3>
    void computeTriangleArea(
        const std::vector<triangle_t *> & elements, const VertexCoordinatesMap<D> & coordinatesMap,
        std::vector<real> & areas)
    {
        // loop on elements
        int e = 0;
        for (const auto & element : elements) {

            // collect vertices
            std::set<const vertex_t *> vertices_set;
            element->getVertices(vertices_set);
            std::vector<const vertex_t *> vertices(vertices_set.begin(), vertices_set.end());

            // compute lengths of three edges
            std::array<real, 3> edges_lengths;
            edges_lengths[0] =
                computeDistance<D>(coordinatesMap[vertices[0]], coordinatesMap[vertices[1]]);
            edges_lengths[1] =
                computeDistance<D>(coordinatesMap[vertices[0]], coordinatesMap[vertices[2]]);
            edges_lengths[2] =
                computeDistance<D>(coordinatesMap[vertices[1]], coordinatesMap[vertices[2]]);

            // sort edges lengths in ascending order
            std::sort(edges_lengths.begin(), edges_lengths.end());

            // a >= b >= c
            real a = edges_lengths[2];
            real b = edges_lengths[1];
            real c = edges_lengths[0];
            real amb = a - b;
            real bmc = b - c;
            real bpc = b + c;

            // compute area of element e
            areas[e] = 0.25 * sqrt((a + bpc) * (c - amb) * (c + amb) * (a + bmc));

            // update elements counter
            ++e;
        }

        // all done
        return;
    }

    template <>
    void computeElementsVolume<triangle_t, 3>(
        const std::vector<triangle_t *> & elements, const VertexCoordinatesMap<3> & coordinatesMap,
        std::vector<real> & volumes)
    {
        return computeTriangleArea(elements, coordinatesMap, volumes);
    }

    // QUESTION:
    // In the case of a bulk element or a line element in 3D the differential in the
    // integral is different (dV, or |r'(d)| dt). Is it a good idea to implement the jacobian, the
    // derivative of the parametrization, the normal times the area differential, as the returned
    // value by a method called, say, differential?

    template <class element_t, int D>
    class ElementSet {

      public:
        ElementSet(
            const std::vector<element_t *> & elements,
            const VertexCoordinatesMap<D> & coordinatesMap) :
            _elements(elements),
            _coordinatesMap(coordinatesMap),
            _jacobians(elements.size(), 0.0)
        {
            // compute the jacobians of the map from reference to current element for each element
            _computeJacobians();
        }

        ElementSet(
            std::vector<element_t *> && elements, const VertexCoordinatesMap<D> & coordinatesMap) :
            _elements(elements),
            _coordinatesMap(coordinatesMap),
            _jacobians(elements.size(), 0.0)
        {
            // compute the jacobians of the map from reference to current element for each element
            _computeJacobians();
        }

        ElementSet(
            const std::vector<element_t *> & elements,
            const VertexCoordinatesMap<D> && coordinatesMap) = delete;

        ElementSet(
            std::vector<element_t *> && elements,
            const VertexCoordinatesMap<D> && coordinatesMap) = delete;

        ~ElementSet() {}

      private:
        // delete default constructor
        ElementSet() = delete;

        // delete copy constructor
        ElementSet(const ElementSet &) = delete;

        // delete move constructor
        ElementSet(const ElementSet &&) = delete;

        // delete assignment operator
        const ElementSet & operator=(const ElementSet &) = delete;

        // delete move assignment operator
        const ElementSet & operator=(const ElementSet &&) = delete;

      public:
        bool sanityCheck()
        {
            bool check = true;
            for (const auto & e : _elements) {
                if (!e->sanityCheck()) {
                    std::cout << "Failed sanity check for element " << e << std::endl;
                    check = false;
                }
            }
            return check;
        }

        inline const std::vector<element_t *> & elements() const { return _elements; }
        inline int nElements() const { return _elements.size(); }
        inline int nVertices() const { return element_t::nVertices(); }
        inline real jacobian(int e) const { return _jacobians[e]; }
        inline const auto & coordinatesVertex(const vertex_t * v) const
        {
            return _coordinatesMap[v];
        }

      private:
        void _computeJacobians()
        {
            return computeElementsVolume<element_t /* element type */, D /* spatial dim*/>(
                _elements, _coordinatesMap, _jacobians);
        }

      private:
        const std::vector<element_t *> _elements;
        const VertexCoordinatesMap<D> & _coordinatesMap;
        std::vector<real> _jacobians;
    };

}    // namespace mito

#endif    //__MITO__ELEMENT_SET__

// end of file
