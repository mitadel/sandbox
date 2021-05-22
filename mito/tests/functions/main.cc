#include "../../math/function.h"
#include <cmath>

using mito::Function;

static const mito::real PI = 4.0 * atan(1.0);
static const mito::real TOL = 1.e-16;

int
main()
{
    // a scalar function
    Function<mito::vector<2>, mito::real> function1(
        [](const mito::vector<2> & x) { return cos(x[0] * x[1]); });

    // a scalar function
    Function<mito::vector<2>, mito::real> function2([](const mito::vector<2> & x) { return 5; });

    // the sum of the two scalar functions
    Function<mito::vector<2>, mito::real> function3 = function1 + function2;

    // a point in the reference configuration
    mito::vector<2> x = { 1.0, PI };

    std::cout << "x = " << x << std::endl;
    std::cout << "f1 = cos(x[0]*x[1]), f1(x) = " << function1(x) << std::endl;
    std::cout << "f2 = 5 , f2(x) = " << function2(x) << std::endl;
    std::cout << "f1(x) + f2(x) = " << function3(x) << std::endl;

    auto function4 = function3 + function3 + function3;
    auto function5 = 3.0 * function3;
    assert(std::fabs(function4(x) - function5(x)) < TOL);

    auto function6 = function4 - function3;
    auto function7 = function5 - function3;
    assert(std::fabs(function6(x) - function7(x)) < TOL);

    auto function8 = (function7 * 0.5) / 0.5;
    assert(std::fabs(function7(x) - function8(x)) < TOL);

    auto function9 = (0.5 / function7) * function7;
    assert(std::fabs(function9(x) - 0.5) < TOL);

    auto function10 = (PI + function7) - function7;
    assert(std::fabs(function10(x) - PI) < TOL);

    auto function11 = (PI + function7) - (2.0 * PI);
    assert(std::fabs(function11(x) - function7(x) + PI) < TOL);

    return 0;
}