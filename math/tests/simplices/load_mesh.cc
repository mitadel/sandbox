#include "../../simplex.h"
#include "../../../mito.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

template<typename T>
void readUntilNextSpace(std::istream * const stream, T & word) {

    // tools for reading file
    std::string line;
    std::stringstream sstream;

    // get line until separator
    std::getline(*stream, line, ' ');

    // convert the string into the type to return
    sstream.clear();
    sstream << line;
    sstream >> word; 

    // all done
    return;
}

void ignoreRestOfLine(std::istream * const stream) {
    // get rest of the line
    std::string line;
    std::getline(*stream, line);
    // all done
    return;
}

template <mito::DIM D>
bool LoadMesh(std::string fileName) {

    // open mesh file
    std::ifstream fileStream;
    fileStream.open(fileName);

    // read dimension of physical space
    int dim = 0;
    readUntilNextSpace(&fileStream, dim);

    // TOFIX
    assert(int(D) == dim);

    // read number of vertices
    int N_vertices = 0; 
    readUntilNextSpace(&fileStream, N_vertices);

    // read number of elements
    int N_elements = 0; 
    readUntilNextSpace(&fileStream, N_elements);

    // read number of element sets
    int N_element_sets = 0; 
    readUntilNextSpace(&fileStream, N_element_sets);
 
    // QUESTION: Not sure that we need this... 
    assert(N_element_sets == 1);

    // fill in vertices
    std::vector<mito::vertex_t*> vertices(N_vertices, nullptr);
    mito::VertexCoordinatesMap<D> vertexCoordinatesMap; 
    for (auto & vertex : vertices) {
        // instantiate new vertex
        vertex = new mito::vertex_t();
        // instantiate new point
        mito::point_t<D> * point = new mito::point_t<D>();
        for (int d = 0; d < D; ++d) {
            // read point coordinates
            readUntilNextSpace(&fileStream, (*point)[d]);
        }
        // associate the new vertex to the new point
        vertexCoordinatesMap.insert(*vertex, std::move(*point));
    }

    // sanity check: the number of vertices in the map is N_vertices
    assert(vertexCoordinatesMap.size() == N_vertices);
    //vertexCoordinatesMap.print();

    // fill in elements
    std::map<std::string, std::vector<const mito::MeshEntity *>> element_sets;
    for (int i = 0; i < N_elements; ++i) {
        int dim_element = 0; 
        readUntilNextSpace(&fileStream, dim_element);

        if (dim_element == 3) {

            int index0 = 0; 
            readUntilNextSpace(&fileStream, index0);

            int index1 = 0; 
            readUntilNextSpace(&fileStream, index1);

            int index2 = 0; 
            readUntilNextSpace(&fileStream, index2);

            mito::segment_t segment0({*vertices[index0], *vertices[index1]});
            mito::segment_t segment1({*vertices[index1], *vertices[index2]});
            mito::segment_t segment2({*vertices[index2], *vertices[index0]});

            // QUESTION: With this implementation, edges have no dignity of their own but are
            //           just 'edges of a triangle'. This will make us lose some information,
            //           because we will see as different segments the same edge seen from two 
            //           different triangles, although of course they have the same vertices.
            const mito::triangle_t * element = new mito::triangle_t({segment0, segment1, segment2});

            // QUESTION: Can the label be more than one? 
            // read label for element
            std::string element_set_id; 
            readUntilNextSpace(&fileStream, element_set_id);

            // add this element to the element_sets labelled with element_set_id 
            element_sets[element_set_id].push_back(element);
        }
    }

    // sanity check: the number of total elements across element sets is N_elements
    int sum = 0;
    for(const auto & element_set : element_sets) {
        sum += element_set.second.size();
    }
    assert(sum == N_elements);

    // sanity check: each element is self-consistent 
    for(const auto & element_set : element_sets) {
        for (const auto & element : element_set.second) {
            assert(element->sanityCheck());
        }
    }

    // finalize file stream
    ignoreRestOfLine(&fileStream);
    assert(fileStream.eof());
    fileStream.close();

    // free memory
    for (const auto & vertex : vertices) {
        delete vertex;
    }

    // free memory
    for(const auto & element_set : element_sets) {
        for (const auto & element : element_set.second) {
            delete element;
        }
    }

    // all done
    return true;
}

int main () {

    LoadMesh<mito::DIM2>("rectangle.summit");
    std::cout << "Loaded mesh" << std::endl;

    return 0;
}
