#include "../mito.h"
#include <cmath>

namespace mito {

    class Gent {

      public:
        Gent(real rho, real kappa, real mu, real Jm) :
            _rho(rho),
            _kappa(kappa),
            _mu(mu),
            _Jm(Jm) {};

        template <dim_t D /*dim*/>
        void Constitutive(const vector<D> & u, const tensor<D> & Du, tensor<D> & P);

      private:
        real _rho;
        real _kappa;
        real _mu;
        real _Jm;
    };

    template <dim_t D /*dim*/>
    void Gent::Constitutive(const vector<D> & u, const tensor<D> & Du, tensor<D> & P)
    {
        // deformation gradient
        tensor<D> F = Du;

        // inverse of deformation gradient
        tensor<D> invF;

        // J = det F
        real detF = ComputeInverse(F, invF);

        // precompute useful quantities
        // J^2 - 1
        real Jsq_minus_1 = detF * detF - 1.;
        // log(J)
        real logJ = log(detF);
        // tr(C)
        real trC = 0.0;
        for (int i = 0; i < D; i++) {
            for (int J = 0; J < D; J++) {
                trC += F[i * D + J] * F[i * D + J];
            }
        }
        // (J^2 -1)/2 - log(J)
        real A = 0.5 * Jsq_minus_1 - logJ;
        // Jm - trC + D /*dim*/
        real C = (_Jm - trC + D /*dim*/);
        // Jm / (Jm - trC + D /*dim*/))
        real B = _Jm / C;

        // fill the first Piola stress tensor
        for (int i = 0; i < D; i++) {
            for (int J = 0; J < D; J++) {
                P[i * D + J] = _mu * B * F[i * D + J]
                             + (2. * _kappa * A * A * A * Jsq_minus_1 - _mu) * invF[J * D + i];
            }
        }

        return;
    }

}

int
main(int argc, char ** argv)
{
    mito::Gent material(1.0 /*rho*/, 1.0 /*kappa*/, 1.0 /*mu*/, 1.0 /*Jm*/);

    mito::vector<3> u = { 0.0, 0.0, 0.0 };
    mito::tensor<3> Du = { 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };
    mito::tensor<3> P = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
    material.Constitutive<3>(u, Du, P);

    mito::vector<2> u2 = { 0.0, 0.0 };
    mito::tensor<2> Du2 = { 1.0, 0.0, 0.0, 1.0 };
    mito::tensor<2> P2 = { 0.0, 0.0, 0.0, 0.0 };
    material.Constitutive<2>(u2, Du2, P2);

    std::cout << "P = " << P << std::endl;
    std::cout << "P2 = " << P2 << std::endl;

    return 0;
}