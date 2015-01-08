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

#ifndef PIC_TONE_MAPPING_LISCHINSKI_MINIMIZATION_HPP
#define PIC_TONE_MAPPING_LISCHINSKI_MINIMIZATION_HPP

#ifndef PIC_DISABLE_EIGEN

#include "externals/Eigen/Sparse"
#include "externals/Eigen/src/SparseCore/SparseMatrix.h"

#include "image.hpp"

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
 * @param alpha
 * @param lambda
 * @param LISCHINSKI_EPSILON
 * @return
 */
Image *LischinskiMinimization(Image *L, Image *g,
                                 Image *omega = NULL, float alpha = 1.0f, float lambda = 0.2f,
                                 float LISCHINSKI_EPSILON = 0.0001f)
{
    if(L == NULL || g == NULL) {
        return NULL;
    }

    if(omega == NULL) {
        omega = L->AllocateSimilarOne();
        *omega = 0.0f;
    }

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

            b[indI] = omega->data[indI] * g->data[indI];

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

            tL.push_back(Eigen::Triplet< double > (indI, indI, omega->data[indI] - sum));
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

    Image *ret = L->AllocateSimilarOne();

    for(int i = 0; i < height; i++) {
        int counter = i * width;

        for(int j = 0; j < width; j++) {
            (*ret)(j, i)[0] = float(x(counter + j));
        }
    }

    return ret;
}

} // end namespace pic

#endif

#endif /* PIC_TONE_MAPPING_LISCHINSKI_MINIMIZATION_HPP */

