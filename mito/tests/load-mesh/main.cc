#include "../../mito.h"
#include "../../mesh/simplex.h"
#include "../../mesh/mesh.h"
#include <fstream>
#include <string>
#include <map>
#include <time.h>

template <mito::dim_t D>
bool
LoadMesh(std::string fileName)
{

    // open mesh file
    std::ifstream fileStream;
    fileStream.open(fileName);
    assert(fileStream.is_open());

    // read dimension of physical space
    int dim = 0;
    fileStream >> dim;

    // assert the template parameter is equal to the dimension of the mesh being read
    assert(int(D) == dim);

    // read number of vertices
    int N_vertices = 0;
    fileStream >> N_vertices;

    // read number of elements
    int N_elements = 0;
    fileStream >> N_elements;

    // read number of element sets
    int N_element_sets = 0;
    fileStream >> N_element_sets;

    // QUESTION: Not sure that we need this...
    assert(N_element_sets == 1);

    // fill in vertices
    std::vector<mito::vertex_t *> vertices(N_vertices, nullptr);
    mito::VertexCoordinatesMap<D> vertexCoordinatesMap;
    for (auto & vertex : vertices) {
        // instantiate new vertex
        vertex = new mito::vertex_t();
        // instantiate new point
        mito::point_t<D> point;
        for (mito::dim_t d = 0; d < D; ++d) {
            // read point coordinates
            fileStream >> point[d];
        }
        // associate the new vertex to the new point
        vertexCoordinatesMap.insert(*vertex, std::move(point));
    }

    // sanity check: the number of vertices in the map is N_vertices
    assert(vertexCoordinatesMap.size() == N_vertices);
    // vertexCoordinatesMap.print();

    // fill in elements
    std::map<std::string, std::vector<const mito::triangle_t *>> element_sets;
    for (int i = 0; i < N_elements; ++i) {
        int element_type = 0;
        fileStream >> element_type;

        if (element_type == 3) {

            int index0 = 0;
            fileStream >> index0;
            --index0;

            int index1 = 0;
            fileStream >> index1;
            --index1;

            int index2 = 0;
            fileStream >> index2;
            --index2;

            mito::segment_t * segment0 =
                new mito::segment_t({ vertices[index0], vertices[index1] });
            mito::segment_t * segment1 =
                new mito::segment_t({ vertices[index1], vertices[index2] });
            mito::segment_t * segment2 =
                new mito::segment_t({ vertices[index2], vertices[index0] });

            // QUESTION: With this implementation, edges have no dignity of their own but are
            //           just 'edges of a triangle'. This will make us lose some information,
            //           because we will see as different segments the same edge seen from two
            //           different triangles, although of course they have the same vertices.
            const mito::triangle_t * element =
                new mito::triangle_t({ segment0, segment1, segment2 });

            // QUESTION: Can the label be more than one?
            // read label for element
            std::string element_set_id;
            fileStream >> element_set_id;

            // add this element to the element_sets labelled with element_set_id
            element_sets[element_set_id].push_back(element);
        }
    }

    // sanity check: the number of total elements across element sets is N_elements
    int sum = 0;
    for (const auto & element_set : element_sets) {
        sum += element_set.second.size();
    }
    assert(sum == N_elements);

    // sanity check: each element is self-consistent
    for (const auto & element_set : element_sets) {
        for (const auto & element : element_set.second) {
            assert(element->sanityCheck());
        }
    }

    // finalize file stream
    fileStream.close();

    // free memory
    for (const auto & vertex : vertices) {
        delete vertex;
    }

    // free memory
    for (const auto & element_set : element_sets) {
        for (const auto & element : element_set.second) {
            for (const auto * entity : element->entities()) {
                delete entity;
            }
            delete element;
        }
    }

    // all done
    return true;
}

int
main()
{
    clock_t t;

    //
    t = clock();
    mito::Mesh<2> mesh("rectangle.summit");
    std::cout << "Loaded mesh (without repeated entities) in " << clock() - t << std::endl;

    //
    t = clock();
    LoadMesh<2>("rectangle.summit");
    std::cout << "Loaded mesh (with repeated entities) in " << clock() - t << std::endl;

    return 0;
}
