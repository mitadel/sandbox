#ifndef __MITO__QUADRATURE__
#define __MITO__QUADRATURE__

#include "../mito.h"
#include "../mesh/simplex.h"

namespace mito {

    template <int D>
    using quadrature_point_t = std::array<mito::real, D>;

    template <class quadrature_t, class element_t, int r>
    struct QuadratureRulesFactory {
        static constexpr auto degreeExactness = r;
        static constexpr int parametricDim = element_t::parametricDim;
        static constexpr auto Get();

      private:
        static constexpr auto GetQuadratureRule();
    };

    struct GAUSS {
        static constexpr auto name = "GAUSS";
    };

    template <int parametricDim, int Q>
    class quadrature_array_t :
        public std::array<std::tuple<quadrature_point_t<parametricDim>, double>, Q> {
      public:
        template <typename... T>
        constexpr quadrature_array_t(T &&... t) :
            std::array<std::tuple<quadrature_point_t<parametricDim>, double>, Q> { std::forward<T>(
                t)... }
        {}

        constexpr auto getPoint(int q) const { return std::get<0>((*this)[q]); }
        constexpr auto getWeight(int q) const { return std::get<1>((*this)[q]); }
    };

    template <class quadrature_t, class element_t, int r>
    constexpr auto QuadratureRulesFactory<quadrature_t, element_t, r>::Get()
    {
        // get the appropriate quadrature rule
        constexpr auto quadrature_rule =
            QuadratureRulesFactory<quadrature_t, element_t, r>::GetQuadratureRule();

        // lambda function to compute the sum of Q quadrature weights at compile time
        constexpr auto sum = [quadrature_rule]<int Q>() consteval->double
        {
            constexpr auto sum_impl = [quadrature_rule]<int q>(auto & sum_ref) consteval->double
            {
                if constexpr (q == -1) {
                    return 0.0;
                } else {
                    return quadrature_rule.getWeight(q)
                         + sum_ref.template operator()<q - 1>(sum_ref);
                }
            };
            return sum_impl.template operator()<Q - 1>(sum_impl);
        };

        // the number of quadrature weights
        constexpr auto Q = quadrature_rule.size();
        // have the compiler compute the sum of the quadrature weights
        constexpr double weightsSum = sum.template operator()<Q>();
        // assert the quadrature weights are a partition of unity
        static_assert(weightsSum == 1.0);

        // return the quadrature rule
        return quadrature_rule;
    }

    template <>
    constexpr auto QuadratureRulesFactory<GAUSS, triangle_t, 1>::GetQuadratureRule()
    {
        return quadrature_array_t<parametricDim, 1 /* nPoints */>(
            { /*{point}, weight*/
              std::make_tuple(
                  quadrature_point_t<parametricDim>({ 1.0 / 3.0, 1.0 / 3.0, 1.0 / 3.0 }), 1.0) });
    }

    template <>
    constexpr auto QuadratureRulesFactory<GAUSS, triangle_t, 2>::GetQuadratureRule()
    {
        return quadrature_array_t<parametricDim, 3 /* nPoints */>(
            { /*{point}, weight*/
              std::make_tuple(
                  quadrature_point_t<parametricDim>({ 2.0 / 3.0, 1.0 / 6.0, 1.0 / 6.0 }),
                  1.0 / 3.0),
              std::make_tuple(
                  quadrature_point_t<parametricDim>({ 1.0 / 6.0, 2.0 / 3.0, 1.0 / 6.0 }),
                  1.0 / 3.0),
              std::make_tuple(
                  quadrature_point_t<parametricDim>({ 1.0 / 6.0, 1.0 / 6.0, 2.0 / 3.0 }),
                  1.0 / 3.0) });
    }

    template <>
    constexpr auto QuadratureRulesFactory<GAUSS, segment_t, 1>::GetQuadratureRule()
    {
        return quadrature_array_t<parametricDim, 1 /* nPoints */>(
            { /*{point}, weight*/
              std::make_tuple(quadrature_point_t<parametricDim>({ 1.0, 1.0 }), 1.0) });
    }

    template <>
    constexpr auto QuadratureRulesFactory<GAUSS, segment_t, 2>::GetQuadratureRule()
    {
        // clang does not allow a constexpr with sqrt(3.0)...
        // constexpr double sqrt3 = sqrt(3.0)
        // ... give the explicit value for sqrt(3) until they come around:
        constexpr double sqrt3 = 1.7320508075688772;

        return quadrature_array_t<parametricDim, 2 /* nPoints */>(
            { /*{point}, weight*/
              std::make_tuple(
                  quadrature_point_t<parametricDim>({ sqrt3 / 3.0, 1.0 - sqrt3 / 3.0 }), 0.5),
              std::make_tuple(
                  quadrature_point_t<parametricDim>({ 1.0 - sqrt3 / 3.0, sqrt3 / 3.0 }), 0.5) });
    }

}    // namespace mito

#endif    //__MITO__QUADRATURE__

// end of file
