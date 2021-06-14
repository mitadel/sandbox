#include "../mito.h"

namespace mito {

    static constexpr auto x0 = std::integral_constant<int, 0> {};
    static constexpr auto x1 = std::integral_constant<int, 1> {};
    static constexpr auto x2 = std::integral_constant<int, 2> {};
    static constexpr auto x3 = std::integral_constant<int, 3> {};

    template <int D>
    class Point {

      private:
        // real_wrap<I>... expands to (real, real, ..., real), D times
        template <size_t>
        using real_wrap = real;

        template <typename = std::make_index_sequence<D>>
        struct tuple_wrap;

        template <size_t... I>
        struct tuple_wrap<std::index_sequence<I...>> {
            using type = std::tuple<real_wrap<I>...>;
        };

        // this expands to tuple<real, real, ..., real>, D times
        using tuple_type = typename tuple_wrap<>::type;

      private:
        template <size_t... I>
        void _initialize(std::index_sequence<I...>)
        {
            ((std::get<I>(_tuple) = 0.0), ...);
        }

        template <size_t... I, class... real>
        void _initialize(std::index_sequence<I...>, real... args)
        {
            ((std::get<I>(_tuple) = args), ...);
        }

      public:
        template <class I>
        real & operator[](I i)
        {
            return std::get<i>(_tuple);
        }

        template <class I>
        const real & operator[](I i) const
        {
            return std::get<i>(_tuple);
        }

        inline Point() : _tuple() { _initialize(std::make_index_sequence<D> {}); }

        template <class... real, typename std::enable_if<sizeof...(real) == D, int>::type = 0>
        Point(real... args) : _tuple()
        {
            _initialize(std::make_index_sequence<D> {}, args...);
        }

      private:
        tuple_type _tuple;
    };

}    // namespace mito

// overload operator<< for Point
std::ostream &
operator<<(std::ostream & os, const mito::Point<1> & x)
{
    os << "(" << x[mito::x0] << ")";
    return os;
}

// overload operator<< for Point
std::ostream &
operator<<(std::ostream & os, const mito::Point<2> & x)
{
    os << "(" << x[mito::x0] << ", " << x[mito::x1] << ")";
    return os;
}

// overload operator<< for Point
std::ostream &
operator<<(std::ostream & os, const mito::Point<3> & x)
{
    os << "(" << x[mito::x0] << ", " << x[mito::x1] << ", " << x[mito::x2] << ")";
    return os;
}

// overload operator<< for Point
std::ostream &
operator<<(std::ostream & os, const mito::Point<4> & x)
{
    os << "(" << x[mito::x0] << ", " << x[mito::x1] << ", " << x[mito::x2] << "," << x[mito::x3]
       << ")";
    return os;
}

// end of file
