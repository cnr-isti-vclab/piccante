/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

*/

#ifndef PIC_ALGORITHMS_LISCHINSKI_MINIMIZATION_HPP
#define PIC_ALGORITHMS_LISCHINSKI_MINIMIZATION_HPP

#ifndef PIC_DISABLE_EIGEN

#ifndef PIC_EIGEN_NOT_BUNDLED
    #include "../externals/Eigen/Sparse"
    #include "../externals/Eigen/src/SparseCore/SparseMatrix.h"
#else
    #include <Eigen/Sparse>
    #include <Eigen/src/SparseCore/SparseMatrix.h>
#endif

#endif

#include "../base.hpp"
#include "../image.hpp"

namespace pic {
/**
*
*	param[0] --> alpha
*	param[1] --> lambda
*
**/

/**
 * @brief LischinskiFunction
 * @param Lcur
 * @param Lref
 * @param param
 * @param LISCHINSKI_EPSILON
 * @return
 */
inline float LischinskiFunction(float Lcur, float Lref, float param[2],
                                float LISCHINSKI_EPSILON = 0.0001f)
{
    return -param[1] / (powf(fabsf(Lcur - Lref), param[0]) + LISCHINSKI_EPSILON);
}

/**
 * @brief LischinskiFunctionGauss
 * @param Lcur
 * @param Lref
 * @param param
 * @return
 */
inline float LischinskiFunctionGauss(float Lcur, float Lref, float param[2])
{
    return expf(-powf(Lcur - Lref, 2.0f) * 10.0f);
}

/**
 * @brief LischinskiMinimization
 * @param L
 * @param g
 * @param omega
 * @param omega_global
 * @param gOut
 * @param alpha
 * @param lambda
 * @param LISCHINSKI_EPSILON
 * @return
 */
PIC_INLINE Image *LischinskiMinimization(Image *L,
                              Image *g,
                              Image *omega = NULL,
                              float omega_global = 1.0f,
                              Image *gOut = NULL,
                              float alpha = 1.0f,
                              float lambda = 0.4f,
                              float LISCHINSKI_EPSILON = 1e-4f)
{
    if(L == NULL || g == NULL) {
        return gOut;
    }

#ifndef PIC_DISABLE_EIGEN
    bool bOmega = (omega == NULL);

    int width = L->width;
    int height = L->height;
    int tot = height * width;

    float param[2];
    param[0] = alpha;
    param[1] = lambda;

    Eigen::VectorXd b, x;
    b = Eigen::VectorXd::Zero(tot);

    #ifdef PIC_DEBUG
        printf("Init matrix...");
    #endif

    std::vector< Eigen::Triplet< double > > tL;

    for(int i = 0; i < height; i++) {
        int tmpInd = i * width;

        for(int j = 0; j < width; j++) {

            float sum = 0.0f;
            float tmp;
            int indJ;
            int indI = tmpInd + j;
            float Lref = L->data[indI];

            float omega_val;
            if(bOmega) {
                omega_val = omega_global;
            } else {
                omega_val = omega->data[indI];
            }

            b[indI] = omega_val * g->data[indI];

            if((i - 1) >= 0) {
                indJ = indI - width;
                tmp = LischinskiFunction(L->data[indJ], Lref, param, LISCHINSKI_EPSILON);
                tL.push_back(Eigen::Triplet< double > (indI, indJ, tmp));
                sum += tmp;
            }

            if((i + 1) < height) {
                indJ = indI + width;
                tmp = LischinskiFunction(L->data[indJ], Lref, param, LISCHINSKI_EPSILON);
                tL.push_back(Eigen::Triplet< double > (indI, indJ, tmp));
                sum += tmp;
            }

            if((j - 1) >= 0) {
                indJ = indI - 1;
                tmp = LischinskiFunction(L->data[indJ], Lref, param, LISCHINSKI_EPSILON);
                tL.push_back(Eigen::Triplet< double > (indI, indJ, tmp));
                sum += tmp;
            }

            if((j + 1) < width) {
                indJ = indI + 1;
                tmp = LischinskiFunction(L->data[indJ], Lref, param, LISCHINSKI_EPSILON);
                tL.push_back(Eigen::Triplet< double > (indI, indJ, tmp));
                sum += tmp;
            }

            tL.push_back(Eigen::Triplet< double > (indI, indI, omega_val - sum));
        }
    }

    #ifdef PIC_DEBUG
        printf("Ok\n");
    #endif

    Eigen::SparseMatrix<double> A = Eigen::SparseMatrix<double>(tot, tot);
    A.setFromTriplets(tL.begin(), tL.end());

    Eigen::SimplicialCholesky<Eigen::SparseMatrix<double> > solver(A);
    x = solver.solve(b);

    if(solver.info() != Eigen::Success) {
        #ifdef PIC_DEBUG
            printf("SOLVER FAILED!\n");
        #endif
        return NULL;
    }

    #ifdef PIC_DEBUG
        printf("SOLVER SUCCESS!\n");
    #endif

    if(gOut == NULL) {
        gOut = g->allocateSimilarOne();
    } else {
        if(!gOut->isSimilarType(g)) {
            gOut = g->allocateSimilarOne();
        }
    }

    for(int i = 0; i < height; i++) {
        int counter = i * width;

        for(int j = 0; j < width; j++) {
            (*gOut)(j, i)[0] = float(x(counter + j));
        }
    }

    return gOut;
#else
    return gOut;
#endif
}

} // end namespace pic

#endif /* PIC_ALGORITHMS_LISCHINSKI_MINIMIZATION_HPP */

