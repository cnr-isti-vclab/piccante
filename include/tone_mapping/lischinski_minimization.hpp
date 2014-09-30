/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

PICCANTE is free software; you can redistribute it and/or modify
under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 3.0 of
the License, or (at your option) any later version.

PICCANTE is distributed in the hope that it will be useful, but
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License
( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.

*/

#ifndef PIC_TONE_MAPPING_LISCHINSKI_MINIMIZATION_HPP
#define PIC_TONE_MAPPING_LISCHINSKI_MINIMIZATION_HPP

#ifndef PIC_DISABLE_EIGEN

#include "externals/Eigen/Sparse"
#include "externals/Eigen/src/SparseCore/SparseMatrix.h"

#include "image_raw.hpp"

namespace pic {
/**
*
*	param[0] --> alpha
*	param[1] --> lambda
*
**/

inline float LischinskiFunction(float Lcur, float Lref, float param[2],
                                float LISCHINSKI_EPSILON = 0.0001f)
{
    return -param[1] / (powf(fabsf(Lcur - Lref), param[0]) + LISCHINSKI_EPSILON);
}

inline float LischinskiFunctionGauss(float Lcur, float Lref, float param[2])
{
    return expf(-powf(Lcur - Lref, 2.0f) * 10.0f);
}

ImageRAW *LischinskiMinimization(ImageRAW *L, ImageRAW *g,
                                 ImageRAW *omega = NULL, float alpha = 1.0f, float lambda = 0.2f,
                                 float LISCHINSKI_EPSILON = 0.0001f)
{
    if(L == NULL || g == NULL) {
        return NULL;
    }

    if(omega == NULL) {
        omega = L->AllocateSimilarOne();
        omega->Assign(0.0f);
    }

    int width = L->width;
    int height = L->height;
    int tot = height * width;

    float param[2];
    param[0] = alpha;
    param[1] = lambda;

    Eigen::VectorXd b, x;
    b = Eigen::VectorXd::Zero(tot);

    printf("Init matrix...");

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

    printf("Ok\n");

    Eigen::SparseMatrix<double> A = Eigen::SparseMatrix<double>(tot, tot);
    A.setFromTriplets(tL.begin(), tL.end());

    Eigen::SimplicialCholesky<Eigen::SparseMatrix<double> > solver(A);
    x = solver.solve(b);

    if(solver.info() != Eigen::Success) {
        printf("SOLVER FAILED!\n");
        return NULL;
    }

    printf("SOLVER SUCCESS!\n");

    ImageRAW *ret = L->AllocateSimilarOne();

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

