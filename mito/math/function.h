
#ifndef __MITO__FUNCTION__
#define __MITO__FUNCTION__

#include "../mito.h"

namespace mito {

    // templatized typedef for functors
    template <typename X, typename Y = Grid<real, 1> /* TOFIX */>
    using functor = std::function<Y(const X &)>;

    // We need a class function to explicitly put the return value Y in the template
    template <typename X, typename Y = Grid<real, 1> /* TOFIX */>
    class Function {

      public:
        inline Function(const functor<X, Y> & f) : _functor(f) {}
        inline Function(functor<X, Y> && f) : _functor(f) {}
        // default constructor
        inline Function() = default;
        // copy constructor
        inline Function(const Function &) = default;
        // move constructor
        inline Function(Function &&) = default;
        // assignment operator
        inline Function & operator=(const Function &) = default;
        // move operator=
        inline Function & operator=(Function &&) = default;

        inline auto operator()(const X & x) const
        {
            // evaluate _f
            return _functor(x);
        }

        inline auto operator[](int i) const
        {
            return mito::Function<X, real>([this, i](const X & x) { return this->_functor(x)[i]; });
        }

        // cast operator from Function<X, Y> to functor<X, Y>
        inline operator functor<X, Y>() const { return _functor; }

      private:
        const functor<X, Y> _functor;
    };

    // Algebraic operations on Function
    // TODO: see if expression templates can be used to avoid copies with intermediate results

    // fa + fb
    template <typename X, typename Y>
    mito::Function<X, Y> operator+(const mito::Function<X, Y> & fA, const mito::Function<X, Y> & fB)
    {
        return mito::Function<X, Y>([fA, fB](const X & x) { return fA(x) + fB(x); });
    }

    // fa * fb
    template <typename X, typename Y>
    mito::Function<X, Y> operator*(const mito::Function<X, Y> & fA, const mito::Function<X, Y> & fB)
    {
        return mito::Function<X, Y>([fA, fB](const X & x) { return fA(x) * fB(x); });
    }

    // y * f (inner product)
    template <
        typename X, typename Y,
        typename std::enable_if<!std::is_same<Y, mito::real>::value, int>::type = 0>
    mito::Function<X, typename type<Y>::value> operator*(
        const Y & y, const mito::Function<X, Y> & f)
    {
        return mito::Function<X, typename type<Y>::value>([y, f](const X & x) { return y * f(x); });
    }

    // f * y (inner product)
    template <
        typename X, typename Y,
        typename std::enable_if<!std::is_same<Y, mito::real>::value, int>::type = 0>
    mito::Function<X, typename type<Y>::value> operator*(
        const mito::Function<X, Y> & f, const Y & y)
    {
        return y * f;
    }

    // a * f
    template <typename X, typename Y>
    mito::Function<X, Y> operator*(const real & a, const mito::Function<X, Y> & f)
    {
        return mito::Function<X, Y>([a, f](const X & x) { return a * f(x); });
    }

    // f * a
    template <typename X, typename Y>
    mito::Function<X, Y> operator*(const mito::Function<X, Y> & f, const real & a)
    {
        return a * f;
    }

    // f / a
    template <typename X, typename Y>
    mito::Function<X, Y> operator/(const mito::Function<X, Y> & f, const real & a)
    {
        return (1.0 / a) * f;
    }

    // -f
    template <typename X, typename Y>
    mito::Function<X, Y> operator-(const mito::Function<X, Y> & f)
    {
        return -1.0 * f;
    }

    // fa - fb
    template <typename X, typename Y>
    mito::Function<X, Y> operator-(const mito::Function<X, Y> & fA, const mito::Function<X, Y> & fB)
    {
        return fA + (-fB);
    }

    // Special algebraic functions for scalar functions
    // a / f
    template <typename X>
    mito::Function<X, mito::real> operator/(const real & a, const mito::Function<X, mito::real> & f)
    {
        return mito::Function<X, mito::real>([a, f](const X & x) { return a / f(x); });
    }

    // f1 / f2
    template <typename X, typename Y>
    mito::Function<X, Y> operator/(
        const mito::Function<X, Y> & f1, const mito::Function<X, mito::real> & f2)
    {
        return mito::Function<X, Y>([f1, f2](const X & x) { return f1(x) / f2(x); });
    }

    // a + f
    template <typename X>
    mito::Function<X, mito::real> operator+(const real & a, const mito::Function<X, mito::real> & f)
    {
        return mito::Function<X, mito::real>([a, f](const X & x) { return a + f(x); });
    }

    // f + a
    template <typename X>
    mito::Function<X, mito::real> operator+(const mito::Function<X, mito::real> & f, const real & a)
    {
        return a + f;
    }

    // a - f
    template <typename X>
    mito::Function<X, mito::real> operator-(const real & a, const mito::Function<X, mito::real> & f)
    {
        return a + (-f);
    }

    // f - a
    template <typename X>
    mito::Function<X, mito::real> operator-(const mito::Function<X, mito::real> & f, const real & a)
    {
        return f + (-a);
    }

    // Binary operators for functor<X, Y> and mito::Function<X, Y>, such as:
    // fa + fb
    template <typename X, typename Y>
    mito::Function<X, Y> operator+(const mito::Function<X, Y> & fA, const functor<X, Y> & fB)
    {
        return fA + mito::Function<X, Y>(fB);
    }

    // fa + fb
    template <typename X, typename Y>
    mito::Function<X, Y> operator+(const functor<X, Y> & fA, const mito::Function<X, Y> & fB)
    {
        return fB + fA;
    }

    // fa * fb
    template <typename X, typename Y>
    mito::Function<X, Y> operator*(const mito::Function<X, Y> & fA, const functor<X, Y> & fB)
    {
        return fA * mito::Function<X, Y>(fB);
    }

    // fa * fb
    template <typename X, typename Y>
    mito::Function<X, Y> operator*(const functor<X, Y> & fA, const mito::Function<X, Y> & fB)
    {
        return fB * fA;
    }

    // fa - fb
    template <typename X, typename Y>
    mito::Function<X, Y> operator-(const mito::Function<X, Y> & fA, const functor<X, Y> & fB)
    {
        return fA - mito::Function<X, Y>(fB);
    }

    // fa - fb
    template <typename X, typename Y>
    mito::Function<X, Y> operator-(const functor<X, Y> & fA, const mito::Function<X, Y> & fB)
    {
        return -(fB - fA);
    }

    // f1 / f2
    template <typename X, typename Y>
    mito::Function<X, Y> operator/(
        const mito::Function<X, Y> & f1, const functor<X, mito::real> & f2)
    {
        return mito::Function<X, Y>([f1, f2](const X & x) { return f1(x) / f2(x); });
    }

    // f1 / f2
    template <typename X, typename Y>
    mito::Function<X, Y> operator/(
        const functor<X, Y> & f1, const mito::Function<X, mito::real> & f2)
    {
        return mito::Function<X, Y>([f1, f2](const X & x) { return f1(x) / f2(x); });
    }

    // Binary operators for Y(const X &) and mito::Function<X, Y>, such as:
    // fa + fb
    template <typename X, typename Y>
    mito::Function<X, Y> operator+(const mito::Function<X, Y> & fA, Y fB(const X &))
    {
        return fA + mito::Function<X, Y>(fB);
    }

    // fa + fb
    template <typename X, typename Y>
    mito::Function<X, Y> operator+(Y fA(const X &), const mito::Function<X, Y> & fB)
    {
        return fB + fA;
    }

    // fa * fb
    template <typename X, typename Y>
    mito::Function<X, Y> operator*(const mito::Function<X, Y> & fA, Y fB(const X &))
    {
        return fA * mito::Function<X, Y>(fB);
    }

    // fa * fb
    template <typename X, typename Y>
    mito::Function<X, Y> operator*(Y fA(const X &), const mito::Function<X, Y> & fB)
    {
        return fB * fA;
    }

    // fa - fb
    template <typename X, typename Y>
    mito::Function<X, Y> operator-(const mito::Function<X, Y> & fA, Y fB(const X &))
    {
        return fA - mito::Function<X, Y>(fB);
    }

    // fa - fb
    template <typename X, typename Y>
    mito::Function<X, Y> operator-(Y fA(const X &), const mito::Function<X, Y> & fB)
    {
        return -(fB - fA);
    }

    // f1 / f2
    template <typename X, typename Y>
    mito::Function<X, Y> operator/(const mito::Function<X, Y> & f1, mito::real f2(const X &))
    {
        return mito::Function<X, Y>([f1, f2](const X & x) { return f1(x) / f2(x); });
    }

    // f1 / f2
    template <typename X, typename Y>
    mito::Function<X, Y> operator/(Y f1(const X &), const mito::Function<X, mito::real> & f2)
    {
        return mito::Function<X, Y>([f1, f2](const X & x) { return f1(x) / f2(x); });
    }
}

#endif    // __MITO__FUNCTION__

// end of file
