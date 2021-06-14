#include <iostream>
// support
#include <cassert>

#include "../../mito.h"
#include "../../elements/quadrature_field.h"

int
main(int argc, char * argv[])
{
    int nElements = 20;
    constexpr int Q = 10;
    mito::QuadratureField<Q, mito::vector_t<2>> quadratureField(nElements);

    std::cout << "Write: " << std::endl;
    // fill-in quadrature field
    for (int e = 0; e < quadratureField.n_elements(); ++e) {
        for (int q = 0; q < quadratureField.n_quad(); ++q) {
            auto myvector = quadratureField(e, q);
            mito::vector_t<2> a = { 1.0, 0.0 };
            mito::vector_t<2> b = { 0.0, 1.0 };
            myvector = e * a + q * b;
            mito::vector_t<2> myvector2 = quadratureField(e, q);
            std::cout << myvector2 << std::endl;
        }
    }

    std::cout << "Read: " << std::endl;
    // read quadrature field
    for (int e = 0; e < quadratureField.n_elements(); ++e) {
        for (int q = 0; q < quadratureField.n_quad(); ++q) {
            mito::vector_t<2> myvector = quadratureField(e, q);
            std::cout << myvector << std::endl;
        }
    }

    // all done
    return 0;
}