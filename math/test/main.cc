#include <cmath>
#include "../math.h"

int main () {

    // a function 
    mito::function<mito::vector<mito::DIM2>, mito::real > f{ 
        [](const mito::vector<mito::DIM2>& x){ return cos(x[0] * x[1]); } };

    // df/dx[0]
    mito::function<mito::vector<mito::DIM2>, mito::real > Dx{ 
        [](const mito::vector<mito::DIM2>& x){ return -sin(x[0] * x[1])* x[1]; } };

    // df/dx[1] 
    mito::function<mito::vector<mito::DIM2>, mito::real > Dy{ 
        [](const mito::vector<mito::DIM2>& x){ return -sin(x[0] * x[1])* x[0]; } };

    // its partial derivatives 
    std::array<mito::function<mito::vector<mito::DIM2>, mito::real>, mito::DIM2> Df = {Dx, Dy};

    // instantiate a function object (NOTE: it should not be mandatory to pass the derivative to 
    // the constructor)
    mito::Function<mito::DIM2> cosFunction(f, Df);

    // a point in the reference configuration
    mito::vector<mito::DIM2> X = {0.0, 0.0};

    // evaluate f and its derivatives
    std::cout << cosFunction(X) << std::endl; 
    std::cout << mito::Grad<mito::DIM2>(cosFunction, X) << std::endl; 
    std::cout << mito::Div<mito::DIM2>(cosFunction, X) << std::endl; 

    return 0;
}