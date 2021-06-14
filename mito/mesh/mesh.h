#include "simplex.h"
#include "vertex_coordinates_map.h"
#include <map>
#include <fstream>

namespace mito {

    template <int D>
    class Mesh {

      private:
        // typedef for a collection of mesh entities
        template <typename T>
        using entity_collection = std::vector<T>;

        // typedef for a collection of simplices of dimension I-1
        template <size_t I>
        using simplex_entity_collection = entity_collection<Simplex<int(I)> *>;

        // simplex_entity_collection<I>... expands to:
        // entity_collection<Simplex<0>*>, entity_collection<Simplex<1>*>, ...,
        //      entity_collection<Simplex<D>*>
        template <typename = std::make_index_sequence<D + 1>>
        struct entities_tuple;

        template <size_t... I>
        struct entities_tuple<std::index_sequence<I...>> {
            using type = std::tuple<simplex_entity_collection<I>...>;
        };

        // this expands to:
        // tuple<entity_collection<Simplex<0>*>, entity_collection<Simplex<1>*>, ...,
        //      entity_collection<Simplex<D>*>
        using entities_tuple_t = typename entities_tuple<>::type;

      private:
        // typedef for a composition map of mesh entities:
        // these maps map:
        //      2 pointers to nodes into a pointer to edge,
        //      3 pointers to edges into a pointer to face, ...
        // std::map<std::array<Simplex<0> *, 2>, Simplex<1> *>  edges composition
        // std::map<std::array<Simplex<1> *, 3>, Simplex<2> *>  faces compositions
        // std::map<std::array<Simplex<2> *, 4>, Simplex<3> *>  volumes compositions
        template <size_t I>
        using composition_map =
            std::map<std::array<Simplex<int(I - 1)> *, I + 1>, Simplex<int(I)> *>;

        template <typename = std::make_index_sequence<D>>
        struct composition_tuple;

        template <size_t... I>
        struct composition_tuple<std::index_sequence<I...>> {
            using type = std::tuple<composition_map<I + 1>...>;
        };

        // this expands to:
        // tuple<composition_map<1>, ..., composition_map<D>
        using composition_tuple_t = typename composition_tuple<>::type;

      public:
        Mesh(std::string meshFileName) : _entities(), _compositions(), _vertexCoordinatesMap()
        {
            _loadMesh(meshFileName);
        }

        template <size_t I>
        void _deleteEntities()
        {
            // delete entities of dimension I
            for (auto element : std::get<I>(_entities)) {
                delete element;
            }

            // all done
            return;
        }

        template <size_t... I>
        void _deleteAllEntities(std::index_sequence<I...>)
        {
            // delete entities of dimension I for all I's in the index sequence
            ((_deleteEntities<I>()), ...);

            // all done
            return;
        }

        ~Mesh()
        {
            // delete all entities from dimension 0 (included) to D (included)
            _deleteAllEntities(std::make_index_sequence<D + 1> {});

            // all done
            return;
        }

      private:
        // delete default constructor
        Mesh() = delete;

        // delete copy constructor
        Mesh(const Mesh &) = delete;

        // delete move constructor
        Mesh(const Mesh &&) = delete;

        // delete assignment operator
        const Mesh & operator=(const Mesh &) = delete;

        // delete move assignment operator
        const Mesh & operator=(const Mesh &&) = delete;

      public:
        bool sanityCheck()
        {
#if 0
            // print summary
            std::cout << "Mesh composition: " << std::endl;
            std::cout << "0: " << std::get<0>(_entities).size() << " entities " << std::endl;
            std::cout << "1: " << std::get<1>(_entities).size() << " entities " << std::endl;
            std::cout << "2: " << std::get<2>(_entities).size() << " entities " << std::endl;
#endif

            // sanity check: each element is self-consistent
            for (const auto & element : std::get<D>(_entities)) {
                if (!element->sanityCheck()) {
                    return false;
                }
            }

            return true;
        }

        template <int I>
        int nEntities() const
        {
            // all done
            return std::get<I>(_entities).size();
        }

      private:
        /**
         * @brief Registers an entity in the composition map
         *
         * @tparam I dimension of the entity to insert
         * @param entity mesh entity to be inserted
         * @return the pair returned by the map insertion:
         *          pair::first is an iterator pointing to either the newly inserted element or
         * to the element with an equivalent key pair::second is true (false) if the entity was
         * inserted (was already in the map)
         */
        template <int I>
        auto _registerEntityComposition(Simplex<I> & entity)
        {
            return std::get<I - 1>(_compositions)
                .insert(std::pair<std::array<Simplex<I - 1> *, I + 1>, Simplex<I> *>(
                    entity.entities(), &entity));
        }

        /**
         * @brief Adds a new composed entity (i.e. edge, face, element) if it is not a repetition
         *         of an equivalent already registered composed entity
         *
         * @tparam I dimension of the composed entity to add (1, 2, ..., D)
         * @param composition entity composition in terms of I + 1 entities of dimension (I - 1)
         * @return mito::Simplex<I>* a pointer either to the newly added entity or to the equivalent
         *                              already registered composed entity
         */
        template <int I>
        mito::Simplex<I> * _addUniqueEntity(std::array<Simplex<I - 1> *, I + 1> && composition)
        {
            // instantiate new entity with this composition
            mito::Simplex<I> * entity = new mito::Simplex<I>(std::move(composition));
            // look up the new entity with its composition and register it if it does not exist yet
            auto ret = _registerEntityComposition(*entity);
            // if the entity did not exist
            if (ret.second == true) {
                // add the entity as a new one
                _addEntity(entity);
            } else {
                // delete the new entity (it was just a repeated entry)
                delete entity;
            }

            // if I == D then ret.second == true, that is there shall be no repetitions of the
            // elements of highest dimension
            assert((I != D) || ret.second);

            // return a pointer to the newly added entity
            return ret.first->second;
        }

        template <int I>
        void _addEntity(Simplex<I> * entity)
        {
            // TOFIX: is push_back expensive even when we reserve the space? No, but we only
            // know in advance how many nodes and elements are in the mesh, not how many edges
            // or faces, so we cannot reserve memory for edges and faces in advance...
            std::get<I>(_entities).push_back(entity);

            // all done
            return;
        }

        void _addVertex(mito::point_t<D> && point)
        {
            // instantiate new vertex
            mito::vertex_t * vertex = new mito::vertex_t();
            // associate the new vertex to the new point
            _vertexCoordinatesMap.insert(*vertex, std::move(point));
            // add the newly created vertex
            _addEntity(vertex);

            // all done
            return;
        }

        template <int I>
        auto _getEntity(int n)
        {
            return std::get<I>(_entities)[n];
        }

        void _readTriangle(std::ifstream & fileStream)
        {
            int index0 = 0;
            fileStream >> index0;
            --index0;

            int index1 = 0;
            fileStream >> index1;
            --index1;

            int index2 = 0;
            fileStream >> index2;
            --index2;

            mito::vertex_t * vertex0 = _getEntity<0>(index0);
            mito::vertex_t * vertex1 = _getEntity<0>(index1);
            mito::vertex_t * vertex2 = _getEntity<0>(index2);

            // TOFIX: compiler cannot deduce template parameter, so specify it explicitly
            mito::segment_t * segment0 = _addUniqueEntity<1>({ vertex0, vertex1 });
            mito::segment_t * segment1 = _addUniqueEntity<1>({ vertex1, vertex2 });
            mito::segment_t * segment2 = _addUniqueEntity<1>({ vertex2, vertex0 });

            // QUESTION: Can the label be more than one?
            // read label for element
            // TOFIX: Ignored for now
            std::string element_set_id;
            fileStream >> element_set_id;

            // TOFIX: compiler cannot deduce template parameter, so specify it explicitly
            _addUniqueEntity<2>({ segment0, segment1, segment2 });

            // all done
            return;
        }

        void _readVertices(std::ifstream & fileStream, int N_vertices)
        {
            // fill in vertices
            for (int n = 0; n < N_vertices; ++n) {
                // instantiate new point
                mito::point_t<D> point;
                for (int d = 0; d < D; ++d) {
                    // read point coordinates
                    fileStream >> point[d];
                }
                _addVertex(std::move(point));
            }
            // _vertexCoordinatesMap.print();

            // all done
            return;
        }

        void _readElements(std::ifstream & fileStream, int N_elements)
        {
            for (int i = 0; i < N_elements; ++i) {
                int element_type = 0;
                fileStream >> element_type;

                if (element_type == 3) {
                    _readTriangle(fileStream);
                } else {
                    std::cout << "Error: Unknown element type" << std::endl;
                }
            }

            // all done
            return;
        }

        void _loadMesh(std::string meshFileName)
        {
            std::cout << "Loading mesh..." << std::endl;

            // open mesh file
            std::ifstream fileStream;
            fileStream.open(meshFileName);
            assert(fileStream.is_open());

            // read dimension of physical space
            int dim = 0;
            fileStream >> dim;

            // assert this mesh object is of same dimension of the mesh being read
            assert(int(D) == dim);

            // read number of vertices
            int N_vertices = 0;
            fileStream >> N_vertices;
            // reserve space for vertices
            std::get<0>(_entities).reserve(N_vertices);

            // read number of elements
            int N_elements = 0;
            fileStream >> N_elements;
            // reserve space for elements
            std::get<D>(_entities).reserve(N_elements);

            // read number of element sets
            int N_element_sets = 0;
            fileStream >> N_element_sets;

            // QUESTION: Not sure that we need this...
            assert(N_element_sets == 1);

            // read the vertices
            _readVertices(fileStream, N_vertices);

            // read the elements
            _readElements(fileStream, N_elements);

            // sanity check: the number of vertices in the map is N_vertices
            assert(nEntities<0>() == N_vertices);

            // sanity check: the number of elements of highest dimension in the map is N_elements
            assert(nEntities<D>() == N_elements);

            // sanity check: run sanity check for all mesh entities in cascade
            assert(sanityCheck());

            // finalize file stream
            fileStream.close();

            // all done
            return;
        }

      private:
        // container to store D+1 containers of d dimensional entities with d = 0, ..., D
        entities_tuple_t _entities;
        // container to store D maps with the composition of i-dimensional entities in terms
        // of arrays of (i-1)-dimensional entities
        composition_tuple_t _compositions;
        // a map between the vertices addresses and a physical point in D-dimensional space
        mito::VertexCoordinatesMap<D> _vertexCoordinatesMap;
    };

}

// end of file
