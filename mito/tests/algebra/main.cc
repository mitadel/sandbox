#include "../../mito.h"

int
main()
{
    mito::vector_t<2> a = { 1.0, 2.0 };
    mito::vector_t<2> b = { 0.0, 2.0 };

#if 0
    auto c = a + 2.0 * b - a - 6.0 * b + a / 2.0 + b + a + b + a + b;
#endif
    // assert characterization of Hilbert spaces
    assert(0.5 * (a + b) * (a + b) + 0.5 * (a - b) * (a - b) == a * a + b * b);

    auto c = a + mito::vector_t<2> { -1.0, 5.0 };
    c += mito::vector_t<2> { 1.0, -5.0 };
    assert(c == a);

    mito::vector_t<2> d = c;
    assert(d == c);

    mito::vector_t<2> e(c);
    assert(e == c);

    e[0] = c[0] + 1.0;
    assert(e != c);

    e = 2.0 * c;
    assert(e - c == c);

    e = -1.0 * c;
    assert((e + c == mito::vector_t<2> { 0, 0 }));

    mito::vector_t<2> f = c / 2.0;
    assert(f + f == c);
    assert(2.0 * f == c);

    // all done
    return 0;
}