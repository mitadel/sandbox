#include "../../mito.h"
#include "../../math/function.h"
#include "../../elements/quadrature_field.h"

template <typename X, typename Y>
class my_class {

  public:
    my_class(const mito::Function<X, Y> & f) : _f(f) {}

    template <template <typename> typename containerT>
    inline auto fill(const containerT<X> & values_x) const
    {
        containerT<Y> values_y(values_x.size());

        auto v_x = values_x.begin();
        auto v_y = values_y.begin();
        while (v_x != values_x.end()) {
            auto x = *v_x++;
            auto & y = *v_y++;
            y = _f(x);
        }

        // all done
        return values_y;
    }

    template <int Q, template <int, typename> typename containerT>
    inline auto fill(const containerT<Q, X> & values_x) const
    {
        containerT<Q, Y> values_y(values_x.size());

        auto v_x = values_x.begin();
        auto v_y = values_y.begin();
        while (v_x != values_x.end()) {
            auto x = *v_x++;
            auto & y = *v_y++;
            y = _f(x);
        }

        // all done
        return values_y;
    }

    mito::Function<X, Y> _f;
};

static constexpr int Q = 3;
template <typename T>
using quadrature_field_t = mito::QuadratureField<Q, T>;

int
main()
{
    constexpr int D = 3;
    int nElements = 10;
    mito::vector<D> v = { 1.0, 2.0, 3.0 };

    quadrature_field_t<mito::vector<D>> quadratureField(nElements);

    for (int e = 0; e < nElements; ++e) {
        for (int q = 0; q < Q; ++q) {
            quadratureField(e, q) = v;
        }
    }

    //
    mito::Function<mito::vector<3>, mito::vector<3>> function([](const mito::vector<3> & x) {
        return mito::vector<3> { cos(x[0] * x[1]), cos(x[0] * x[1]), cos(x[0] * x[1]) };
    });

    my_class my_obj(function);
    auto quadratureField_y = my_obj.fill(quadratureField);

    std::vector<mito::vector<D>> my_vector_x(Q * nElements);
    for (auto & v_i : my_vector_x) {
        v_i = v;
    }
    auto my_vector_y = my_obj.fill(my_vector_x);

    auto v_a = quadratureField_y.begin();
    auto v_b = my_vector_y.begin();
    while (v_a != quadratureField_y.end()) {
        auto x = *v_a++;
        auto & y = *v_b++;
        assert(x == y);
    }

    quadratureField_y(0, 0);

    return 0;
}