#include <algorithm>
#include <ranges>
#include "../../math/point.h"

using mito::DIM1;
using mito::DIM2;
using mito::DIM3;
using mito::Point;
using mito::x0;
using mito::x1;
using mito::x2;
using mito::x3;

template <int I>
constexpr auto
index();

template <>
constexpr auto
index<0>()
{
    return std::integral_constant<int, 0> {};
}
template <>
constexpr auto
index<1>()
{
    return std::integral_constant<int, 1> {};
}
template <>
constexpr auto
index<2>()
{
    return std::integral_constant<int, 2> {};
}
template <>
constexpr auto
index<3>()
{
    return std::integral_constant<int, 3> {};
}

template <std::size_t N, std::size_t... I>
constexpr auto create_array_impl(std::index_sequence<I...>)
{
    return std::array<int, N> { I... };
}

template <std::size_t N>
constexpr auto
create_array()
{
    return create_array_impl<N>(std::make_index_sequence<N> {});
}

constexpr auto
integralConstant(auto i)
{
    return index<i>();
}

int
main()
{

    Point<DIM1> a(10.0);
    std::cout << a << std::endl;

    Point<DIM2> b(10.0, 2.0);
    std::cout << b << std::endl;

    Point<DIM3> c(10.0, 2.0, 1.0);
    std::cout << c << std::endl;

    std::vector<int> nums { 1 };
    std::as_const(nums);

    // constexpr auto a = std::integral_constant<int, index>{};

    std::cout << c[index<0>()] << std::endl;

    constexpr int indexx = 0;
    std::cout << c[index<indexx>()] << std::endl;

    // std::cout << c[0] << std::endl;

    return 0;
}