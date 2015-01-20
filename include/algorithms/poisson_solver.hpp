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

#ifndef PIC_ALGORITHMS_POISSON_SOLVER_HPP
#define PIC_ALGORITHMS_POISSON_SOLVER_HPP

#ifndef PIC_DISABLE_EIGEN

#include "externals/Eigen/Sparse"
#include "externals/Eigen/src/SparseCore/SparseMatrix.h"

#include "image.hpp"

namespace pic {

/**
 * @brief PoissonSolver
 * @param f
 * @param ret
 * @return
 */
Image *PoissonSolver(Image *f, Image *ret = NULL)
{
    if(f == NULL) {
        return NULL;
    }

    //Allocating the output
    if(ret == NULL) {
        ret = f->AllocateSimilarOne();
    }

    int width = f->width;
    int height = f->height;
    int tot = height * width;

    #ifdef PIC_DEBUG
        printf("Init matrix...");
    #endif

    std::vector< Eigen::Triplet< double > > tL;

    for(int i = 0; i < height; i++) {
        int tmpI = i * width;

        for(int j = 0; j < width; j++) {
            int indI = tmpI + j;

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

    //Solving the system for each color channel
    Eigen::SparseMatrix<double> A = Eigen::SparseMatrix<double>(tot, tot);
    A.setFromTriplets(tL.begin(), tL.end());
    Eigen::SimplicialCholesky<Eigen::SparseMatrix<double> > solver(A);

    for(int k=0; k< f->channels; k++) {

        Eigen::VectorXd b, x;
        b = Eigen::VectorXd::Zero(tot);

        //copying values from f to b
        for(int i = 0; i < height; i++) {
            int tmpI = i * width;
            for(int j = 0; j < width; j++) {
                int indI = (tmpI + j);
                b[indI] = - f->data[indI * f->channels + k];
            }
        }

        x = solver.solve(b);

        if(solver.info() != Eigen::Success) {
            #ifdef PIC_DEBUG
                printf("SOLVER FAILED!\n");
            #endif

            return ret;
        }

        #ifdef PIC_DEBUG
            printf("SOLVER SUCCESS!\n");
        #endif

        for(int i = 0; i < height; i++) {
            int tmpI = i * width;

            for(int j = 0; j < width; j++) {
                (*ret)(j, i)[k] = float(x(tmpI + j));
            }
        }
    }

    return ret;
}

} // end namespace pic

#endif

#endif /* PIC_ALGORITHMS_POISSON_SOLVER_HPP */

