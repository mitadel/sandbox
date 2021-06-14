#ifndef __MITO__FIELDS__
#define __MITO__FIELDS__

#include "function.h"
#include "../mito.h"
#include "../elements/quadrature_field.h"

namespace mito {

    // TODO: Add operator+ for scalar fields and reals

    // f(X,t) with (X \in R^D, t \in R) -> Y
    template <typename X, typename Y = Grid<real, 1> /* TOFIX */>
    class Field {

        // dimension of the X space
        // static constexpr int D = size<X>::value;
        static constexpr int D = X::S;

        // typedef for a scalar valued function
        using function_t = Function<X, Y>;
        using functor_t = functor<X, Y>;

      public:
        // constructors with function_t
        Field(const function_t & f) : _f(f), _Df() {}
        Field(function_t && f) : _f(f), _Df() {}
        Field(const function_t & f, const std::array<function_t, D> & Df) : _f(f), _Df(Df) {}
        Field(const function_t & f, std::array<function_t, D> && Df) : _f(f), _Df(Df) {}
        Field(function_t && f, std::array<function_t, D> && Df) : _f(f), _Df(Df) {}

        // constructors with functor_t
        Field(const functor_t & f) : _f(f), _Df() {}
        Field(functor_t && f) : _f(f), _Df() {}
        Field(const functor_t & f, const std::array<functor_t, D> & Df) : _f(f), _Df(Df) {}
        Field(const functor_t & f, std::array<functor_t, D> && Df) : _f(f), _Df(Df) {}
        Field(functor_t && f, std::array<functor_t, D> && Df) : _f(f), _Df(Df) {}

        // default move constructor
        Field(Field &&) = default;

        // default move operator=
        Field & operator=(Field &&) = default;

        // default copy constructor
        Field(const Field &) = default;

        // default assignment operator
        Field & operator=(const Field &) = default;

        // delete default constructor
        Field() = delete;

        // destructor
        ~Field() {}

        inline auto operator()(const X & x) const
        {
            // evaluate _f
            return _f(x);
        }

        // TODO: Note that now this operator() is the same thing as the following but the only thing
        // that changes is the container structure
        inline auto operator()(const std::vector<X> & x) const
        {
            std::vector<Y> values(x.size());
            // evaluate operator() at all elements of X
            for (int i = 0; i < (int) x.size(); ++i) {
                values[i] = operator()(x[i]);
            }
            return values;
        }

        template <int Q>
        inline auto operator()(const quadrature_field_t<Q, X> & x) const
        {
            quadrature_field_t<Q, Y> values(x.n_elements());

            // evaluate operator() at all elements of x
            for (int e = 0; e < values.n_elements(); ++e) {
                for (int q = 0; q < Q; ++q) {
                    values(e, q) = operator()(x(e, q));
                }
            }

            // all done
            return values;
        }

      public:
        // accessor for function
        inline const auto & f() const { return _f; }

        // accessor for function partial derivatives
        inline const auto & Df(int i) const
        {
            // assert there exists the i-th partial derivative
            assert(i < (int) _Df.size());
            // return the i-th partial derivative
            return _Df[i];
        }

      private:
        // the function
        function_t _f;
        // the derivatives of f with respect to X (position in the reference configuration)
        std::array<function_t, D> _Df;
    };

    template <int D, int N>
    using VectorField = Field<vector<D>, vector<N>>;

    template <int D>
    using ScalarField = Field<vector<D>, Grid<mito::real, 1> /*TOFIX */>;

    template <typename X, typename Y, std::size_t... I>
    inline auto _dSum(
        const Field<X, Y> & fieldA, const Field<X, Y> & fieldB, std::index_sequence<I...>)
    {
        // dimension of the X space
        static constexpr int D = X::S;

        std::array<Function<X, Y>, D> Df = { (fieldA.Df(I) + fieldB.Df(I))... };
        return Df;
    }

    template <typename X, typename Y>
    auto operator+(const Field<X, Y> & fieldA, const Field<X, Y> & fieldB)
    {
        // dimension of the X space
        static constexpr int D = X::S;

        return Field<X, Y>(
            fieldA.f() + fieldB.f(), _dSum(fieldA, fieldB, std::make_index_sequence<D> {}));
    }

    // helper function to compute the gradient of a vector field with respect to the reference
    // configuration (template with index sequence)
    template <int D, std::size_t... I>
    inline auto _grad(const ScalarField<D> & field, const vector<D> & x, std::index_sequence<I...>)
    {
        // all done
        return vector<D> { field.Df(I)(x)... };
    }

    // function to compute the gradient of a scalar field with respect to the reference
    // configuration at point x
    template <int D>
    inline auto grad(const ScalarField<D> & field, const vector<D> & x)
    {
        return _grad(field, x, std::make_index_sequence<D> {});
    }

    // helper function to compute the gradient of a vector field with respect to the reference
    // configuration (template with index sequence)
    template <int D, std::size_t... I>
    inline VectorField<D, D> _grad(const ScalarField<D> & field, std::index_sequence<I...>)
    {
        // TOFIX: capturing field by copy or by reference? Is it better to capture
        // {field.Df(I)...} intead? Is it possible to do so?
        return VectorField<D, D>(mito::Function<vector<D>, vector<D>>(
            [field](const vector<D> & x) { return vector<D> { field.Df(I)(x)... }; }));
    }

    // function to compute the gradient of a vector field with respect to the reference
    // configuration
    template <int D>
    inline VectorField<D, D> grad(const ScalarField<D> & field)
    {
        return _grad(field, std::make_index_sequence<D> {});
    }

    // function to compute the Divergence of a vector field at point X
    template <int D>
    inline real div(const VectorField<D, D> & field, const vector<D> & X)
    {
        real result = 0.0;
        for (int i = 0; i < D; ++i) {
            result += field.Df(i)(X)[i];
        }
        return result;
    }

    // helper function to compute the divergence of a vector field with respect to the reference
    // configuration at point X (template with index sequence)
    template <int D, std::size_t... I>
    inline ScalarField<D> _div(const VectorField<D, D> & field, std::index_sequence<I...>)
    {
        // TOFIX: capturing field by copy or by reference? Is it better to capture
        // {field.Df(I)...} intead? Is it possible to do so?
        return ScalarField<D>(mito::Function<vector<D>>(
            [field](const vector<D> & x) { return (field.Df(I)(x)[I] + ...); }));
    }

    // function to compute the divergence of a vector field with respect to the reference
    // configuration at point X
    template <int D>
    inline ScalarField<D> div(const VectorField<D, D> & field)
    {
        return _div(field, std::make_index_sequence<D> {});
    }
}

#endif    //__MITO__FIELDS__

// end of file
