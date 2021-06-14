#ifndef __MITO__
#define __MITO__

#include <array>
#include <vector>
#include <functional>
#include <iostream>
#include <cassert>
#include <math.h>

#include "math/algebra.h"

// https://stackoverflow.com/questions/4295432/typedef-function-pointer
// https://stackoverflow.com/questions/7787500/how-to-write-a-function-that-takes-a-functor-as-an-argument

// TOFIX: Consider using tuples when possible as opposed to std::array and std::vector

namespace mito {

    using dim_t = size_t;
    // typedef for scalars
    using real = double;

    template <int D, class T = real>
    using vector_t = Grid<T, D>;

    // TOFIX
    template <class T = real>
    using scalar_t = Grid<T, 1>;

    template <int D1, int D2 = D1, class T = real>
    using tensor_t = Grid<T, D1, D2>;

    // TOFIX: remove redundant {vector}
    // typedef for vectors
    template <dim_t D, class T = real>
    using vector = vector_t<D, T>;

    // TOFIX: remove redundant {tensor}
    // typedef for tensors
    template <dim_t D1, dim_t D2 = D1>
    using tensor = tensor_t<D1, D2, real>;

    template <dim_t D>
    using point_t = mito::vector<D>;    // Point<D>;

    // templatized typedef for fields
    template <typename X, typename Y>
    using field = Y (*)(const X &, real);
}

namespace mito {

    // overload operator<< for vectors and tensors
    std::ostream & operator<<(std::ostream & os, const mito::vector<3> & x)
    {
        os << "(" << x[0] << ", " << x[1] << ", " << x[2] << ")";
        return os;
    }

    std::ostream & operator<<(std::ostream & os, const mito::vector<2> & x)
    {
        os << "(" << x[0] << ", " << x[1] << ")";
        return os;
    }

    std::ostream & operator<<(std::ostream & os, const mito::vector<1> & x)
    {
        os << x[0];
        return os;
    }

    std::ostream & operator<<(std::ostream & os, const mito::tensor<3> & x)
    {
        os << "(" << x[0] << ", " << x[1] << ", " << x[2] << "; " << x[3] << ", " << x[4] << ", "
           << x[5] << "; " << x[6] << ", " << x[7] << ", " << x[8] << ")";
        return os;
    }

    std::ostream & operator<<(std::ostream & os, const mito::tensor<2> & x)
    {
        os << "(" << x[0] << ", " << x[1] << "; " << x[2] << ", " << x[3] << ")";
        return os;
    }

    template <typename X>
    std::ostream & operator<<(std::ostream & os, const std::vector<X> & x)
    {

        if (x.size() == 0) {
            os << "[]";
            return os;
        }

        os << "[" << x[0];

        for (auto i = 1; i < x.size(); ++i) {
            os << ", " << x[i];
        }

        os << "]";

        return os;
    }

    template <typename X, long unsigned int N>
    std::ostream & operator<<(std::ostream & os, const std::array<X, N> & x)
    {

        if (N == 0) {
            os << "[]";
            return os;
        }

        os << "[" << x[0];

        for (long unsigned int i = 1; i < N; ++i) {
            os << ", " << x[i];
        }

        os << "]";

        return os;
    }
}

namespace mito {

    // TODO: Check that this implementation makes sense
    // Algebraic operations on vectors, tensors, ...

    // mito::vector times scalar
    template <dim_t D, typename T, std::size_t... I>
    inline auto _vector_times_scalar(
        const mito::real & a, mito::vector<D, T> & y, std::index_sequence<I...>)
    {
        ((y[I] *= a), ...);
        return;
    }
    template <dim_t D, typename T>
    inline mito::vector<D, T> & operator*(const mito::real & a, mito::vector<D, T> && y)
    {
        _vector_times_scalar(a, y, std::make_index_sequence<D> {});
        return y;
    }
    template <dim_t D, typename T>
    inline mito::vector<D, T> & operator*(const mito::real & a, mito::vector<D, T> & y)
    {
        _vector_times_scalar(a, y, std::make_index_sequence<D> {});
        return y;
    }
    template <dim_t D, typename T>
    inline mito::vector<D, T> & operator*(mito::vector<D, T> && y, const mito::real & a)
    {
        return a * y;
    }
    template <dim_t D, typename T>
    inline mito::vector<D, T> & operator*(mito::vector<D, T> & y, const mito::real & a)
    {
        return a * y;
    }

    // mito::vector inner product
    template <dim_t D, typename T, std::size_t... I>
    inline T _vector_inner_product(
        const mito::vector<D, T> & y1, const mito::vector<D, T> & y2, std::index_sequence<I...>)
    {
        T result(0);
        ((result += y1[I] * y2[I]), ...);
        return result;
    }
    template <dim_t D, typename T>
    inline T operator*(const mito::vector<D, T> & y1, const mito::vector<D, T> & y2)
    {
        return _vector_inner_product(y1, y2, std::make_index_sequence<D> {});
    }
    template <dim_t D, typename T>
    inline T operator*(mito::vector<D, T> && y1, const mito::vector<D, T> & y2)
    {
        return _vector_inner_product(y1, y2, std::make_index_sequence<D> {});
    }
    template <dim_t D, typename T>
    inline T operator*(const mito::vector<D, T> & y1, mito::vector<D, T> && y2)
    {
        return _vector_inner_product(y1, y2, std::make_index_sequence<D> {});
    }

    template <dim_t D, typename T>
    inline T operator*(mito::vector<D, T> && y1, mito::vector<D, T> && y2)
    {
        return _vector_inner_product(y1, y2, std::make_index_sequence<D> {});
    }

    // sum of mito::vectors
    template <dim_t D, typename T, std::size_t... I>
    inline mito::vector<D, T> _vector_sum(
        const mito::vector<D, T> & y1, const mito::vector<D, T> & y2, std::index_sequence<I...>)
    {
        mito::vector<D, T> result;
        ((result[I] = y1[I] + y2[I]), ...);
        return result;
    }
    template <dim_t D, typename T>
    inline mito::vector<D, T> operator+(
        const mito::vector<D, T> & y1, const mito::vector<D, T> & y2)
    {
        return _vector_sum(y1, y2, std::make_index_sequence<D> {});
    }
    template <dim_t D, typename T>
    inline mito::vector<D, T> operator+(mito::vector<D, T> && y1, const mito::vector<D, T> & y2)
    {
        return _vector_sum(y1, y2, std::make_index_sequence<D> {});
    }
    template <dim_t D, typename T>
    inline mito::vector<D, T> operator+(const mito::vector<D, T> & y1, mito::vector<D, T> && y2)
    {
        return _vector_sum(y1, y2, std::make_index_sequence<D> {});
    }
    template <dim_t D, typename T>
    inline mito::vector<D, T> operator+(mito::vector<D, T> && y1, mito::vector<D, T> && y2)
    {
        return _vector_sum(y1, y2, std::make_index_sequence<D> {});
    }

    // mito::vector operator-
    template <dim_t D, typename T, std::size_t... I>
    inline mito::vector<D, T> _vector_minus(mito::vector<D, T> & y, std::index_sequence<I...>)
    {
        ((y[I] = -y[I]), ...);
        return y;
    }
    template <dim_t D, typename T>
    inline mito::vector<D, T> operator-(const mito::vector<D, T> & y)
    {
        return _vector_minus(y, std::make_index_sequence<D> {});
    }
    template <dim_t D, typename T>
    inline mito::vector<D, T> operator-(const mito::vector<D, T> && y)
    {
        return _vector_minus(y, std::make_index_sequence<D> {});
    }
    template <dim_t D, typename T>
    inline mito::vector<D, T> operator-(
        const mito::vector<D, T> & y1, const mito::vector<D, T> & y2)
    {
        return y1 + (-y2);
    }
    template <dim_t D, typename T>
    inline mito::vector<D, T> operator-(mito::vector<D, T> && y1, const mito::vector<D, T> & y2)
    {
        return y1 + (-y2);
    }
    template <dim_t D, typename T>
    inline mito::vector<D, T> operator-(const mito::vector<D, T> & y1, mito::vector<D, T> && y2)
    {
        return y1 + (-y2);
    }
    template <dim_t D, typename T>
    inline mito::vector<D, T> operator-(mito::vector<D, T> && y1, mito::vector<D, T> && y2)
    {
        return y1 + (-y2);
    }

    // factorial
    template <dim_t D>
    int Factorial()
    {
        return D * Factorial<dim_t(D - 1)>();
    }
    template <>
    int Factorial<1>()
    {
        return 1;
    }

    real ComputeDeterminant(const tensor<4> & A)
    {
        return A[1] * A[11] * A[14] * A[4] - A[1] * A[10] * A[15] * A[4]
             - A[11] * A[13] * A[2] * A[4] + A[10] * A[13] * A[3] * A[4]
             - A[0] * A[11] * A[14] * A[5] + A[0] * A[10] * A[15] * A[5]
             + A[11] * A[12] * A[2] * A[5] - A[10] * A[12] * A[3] * A[5]
             - A[1] * A[11] * A[12] * A[6] + A[0] * A[11] * A[13] * A[6]
             + A[1] * A[10] * A[12] * A[7] - A[0] * A[10] * A[13] * A[7]
             - A[15] * A[2] * A[5] * A[8] + A[14] * A[3] * A[5] * A[8] + A[1] * A[15] * A[6] * A[8]
             - A[13] * A[3] * A[6] * A[8] - A[1] * A[14] * A[7] * A[8] + A[13] * A[2] * A[7] * A[8]
             + A[15] * A[2] * A[4] * A[9] - A[14] * A[3] * A[4] * A[9] - A[0] * A[15] * A[6] * A[9]
             + A[12] * A[3] * A[6] * A[9] + A[0] * A[14] * A[7] * A[9] - A[12] * A[2] * A[7] * A[9];
    }

    real ComputeDeterminant(const tensor<3> & A)
    {
        return A[0] * (A[4] * A[8] - A[5] * A[7]) - A[1] * (A[3] * A[8] - A[5] * A[6])
             + A[2] * (A[3] * A[7] - A[4] * A[6]);
    }

    real ComputeDeterminant(const tensor<2> & A) { return A[0] * A[3] - A[1] * A[2]; }

    real ComputeInverse(const tensor<3> & A, tensor<3> & invA)
    {
        real det = ComputeDeterminant(A);
        assert(det != 0.0);

        real detinv = 1.0 / det;
        invA[0] = detinv * (A[4] * A[8] - A[5] * A[7]);
        invA[1] = detinv * (-A[1] * A[8] + A[2] * A[7]);
        invA[2] = detinv * (A[1] * A[5] - A[2] * A[4]);
        invA[3] = detinv * (-A[3] * A[8] + A[5] * A[6]);
        invA[4] = detinv * (A[0] * A[8] - A[2] * A[6]);
        invA[5] = detinv * (-A[0] * A[5] + A[2] * A[3]);
        invA[6] = detinv * (A[3] * A[7] - A[4] * A[6]);
        invA[7] = detinv * (-A[0] * A[7] + A[1] * A[6]);
        invA[8] = detinv * (A[0] * A[4] - A[1] * A[3]);

        return det;
    }

    real ComputeInverse(const tensor<2> & A, tensor<2> & invA)
    {
        real det = ComputeDeterminant(A);
        assert(det != 0.0);

        real detinv = 1.0 / det;
        invA[0] = detinv * (A[3]);
        invA[1] = detinv * (-A[1]);
        invA[2] = detinv * (-A[2]);
        invA[3] = detinv * (A[0]);

        return det;
    }
}

// traits on mito types
namespace mito {
    template <typename T>
    struct size {
        static constexpr int value = T::S;
    };

    template <>
    struct size<int> {
        static constexpr int value = 1;
    };

    template <>
    struct size<bool> {
        static constexpr int value = 1;
    };

    template <>
    struct size<double> {
        static constexpr int value = 1;
    };

    template <typename T>
    struct type {
        using value = typename T ::type;
    };

    template <>
    struct type<int> {
        using value = int;
    };

    template <>
    struct type<bool> {
        using value = bool;
    };

    template <>
    struct type<double> {
        using value = double;
    };
}

namespace mito {
    template <typename X, typename Y>
    int AddDirichletBC(std::string boundaryLabel, const field<X, Y> & myF)
    {
        std::cout << "Setting Dirichlet BC on " << boundaryLabel << std::endl;

        X x;
        std::fill(x.begin(), x.end(), 0.0);

        real t = 0.0;

        std::cout << "\tEvaluating BC field at " << x << std::endl;

        auto y = myF(x, t);

        std::cout << "\tmyF(x, t) = " << y << std::endl;

        return 0;
    }
}
#endif    //__MITO__
// end of file
