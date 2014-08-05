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

#ifndef PIC_ALGORITHMS_POISSON_SOLVER_HPP
#define PIC_ALGORITHMS_POISSON_SOLVER_HPP

#ifndef PIC_DISABLE_EIGEN

#include "externals/Eigen/Sparse"
#include "externals/Eigen/src/SparseCore/SparseMatrix.h"

#include "image_raw.hpp"

namespace pic {

ImageRAW *PoissonSolver(ImageRAW *f, ImageRAW *ret)
{
    if(f == NULL) {
        return NULL;
    }

    int width = f->width;
    int height = f->height;
    int tot = height * width;

    Eigen::VectorXd b, x;
    b = Eigen::VectorXd::Zero(tot);

    #ifdef PIC_DEBUG
        printf("Init matrix...");
    #endif

    std::vector< Eigen::Triplet< double > > tL;

    for(int i = 0; i < height; i++) {
        int tmpI = i * width;

        for(int j = 0; j < width; j++) {
            int indI = tmpI + j;
            b[indI] = -f->data[indI];

            tL.push_back(Eigen::Triplet< double > (indI, indI, 4.0f));

            if(((indI + 1) < tot) &&
               ((indI % width) != (width - 1))) {

                tL.push_back(Eigen::Triplet< double > (indI, indI + 1, -1.0f));
                tL.push_back(Eigen::Triplet< double > (indI + 1, indI, -1.0f));
            }
        }
    }

    for(int i = 0; i < (tot - width); i++) {
        tL.push_back(Eigen::Triplet< double > (i + width, i         , -1.0f));
        tL.push_back(Eigen::Triplet< double > (i        , i +  width, -1.0f));
    }

    #ifdef PIC_DEBUG
        printf("Ok\n");
    #endif

    //Solving the linear system
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

    //Copy from x to ret
    if(ret == NULL) {
        ret = f->AllocateSimilarOne();
    }

    for(int i = 0; i < height; i++) {
        int tmpI = i * width;

        for(int j = 0; j < width; j++) {
            (*ret)(j, i)[0] = x(tmpI + j);
        }
    }

    return ret;
}

} // end namespace pic

#endif

#endif /* PIC_ALGORITHMS_POISSON_SOLVER_HPP */

