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

#ifndef PIC_ALGORITHMS_POISSON_IMAGE_EDITING_HPP
#define PIC_ALGORITHMS_POISSON_IMAGE_EDITING_HPP

#ifndef PIC_DISABLE_EIGEN

#include "externals/Eigen/Sparse"
#include "externals/Eigen/src/SparseCore/SparseMatrix.h"

#include "image.hpp"
#include "filtering/filter_laplacian.hpp"

namespace pic {

/**
 * @brief PoissonImageEditing
 * @param source
 * @param target
 * @param mask
 * @param ret
 * @return
 */
Image *PoissonImageEditing(Image *source, Image *target, bool *mask, Image *ret = NULL)
{
    if((source == NULL) || (target == NULL) || (mask == NULL)) {
        return NULL;
    }

    //Allocating the output
    if(ret == NULL) {
        ret = target->Clone();
    }

    int width  = target->width;
    int height = target->height;

    #ifdef PIC_DEBUG
        printf("Init matrix...");
    #endif

    Image *lap_source = FilterLaplacian::Execute(source, NULL);

    std::vector< Eigen::Triplet< double > > tL;

    //indices pass
    int *index = new int[width * height];
    int count = 0;
    for(int i = 0; i < height; i++) {
        int tmpI = i * width;

        for(int j = 0; j < width; j++) {
            int indI = tmpI + j;

            if(mask[indI]) {
                index[indI] = count;
                count++;
            } else {
                index[indI] = 0;
            }
        }
    }

    //matrix A pass
    count = 0;
    for(int i = 0; i < height; i++) {
        int tmpI = i * width;

        for(int j = 0; j < width; j++) {
            int indI = tmpI + j;

            if(mask[indI]) {

                float n = 0.0f;
                if(i < (width - 1) && mask[indI + 1]) {
                    n += 1.0f;
                    tL.push_back(Eigen::Triplet< double > (count, index[indI + 1], -1.0));
                }

                if(i > 0 && mask[indI - 1]) {
                    n += 1.0f;
                    tL.push_back(Eigen::Triplet< double > (count, index[indI - 1], -1.0));
                }

                if(j < (height - 1) && mask[indI + width]) {
                    n += 1.0f;
                    tL.push_back(Eigen::Triplet< double > (count, index[indI + width], -1.0));
                }

                if(j > 0 && mask[indI - width]) {
                    n += 1.0f;
                    tL.push_back(Eigen::Triplet< double > (count, index[indI - width], -1.0));
                }

                tL.push_back(Eigen::Triplet< double > (count, count , 4.0));

                count++;
            }
        }
    }

    int tot = count;
    Eigen::SparseMatrix<double> A = Eigen::SparseMatrix<double>(tot, tot);
    A.setFromTriplets(tL.begin(), tL.end());

    #ifdef PIC_DEBUG
        printf("Ok\n");
    #endif

    //Solving the system for each color channel
    Eigen::SimplicialCholesky<Eigen::SparseMatrix<double> > solver(A);

    for(int k=0; k< target->channels; k++) {

        Eigen::VectorXd b, x;
        b = Eigen::VectorXd::Zero(tot);

        //assigning values to b
        int count = 0;
        for(int i = 0; i < height; i++) {
            int tmpI = i * width;

            for(int j = 0; j < width; j++) {
                int indI = (tmpI + j);

                if(mask[indI]) {

                    b[count] = -(*lap_source)(j, i)[k];

                    if(i < (width - 1) && !mask[indI + 1]) {
                        b[count] += (*target)(j + 1, i)[k];
                    }

                    if(i > 0 && !mask[indI - 1]) {
                        b[count] += (*target)(j - 1, i)[k];
                    }

                    if(i < (height - 1) && !mask[indI + width]) {
                        b[count] += (*target)(j, i + 1)[k];
                    }

                    if(j > 0 && !mask[indI - width]) {
                        b[count] += (*target)(j, i - 1)[k];
                    }

                    count++;
                }
            }
        }

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

        count = 0;
        for(int i = 0; i < height; i++) {
            int tmpI = i * width;

            for(int j = 0; j < width; j++) {
                int indI = (tmpI + j);

                if(mask[indI]) {
                    float val = float(x(count));
                    (*ret)(j, i)[k] = val > 0.0f ? val : 0.0f;
                    count++;
                }
            }
        }
    }

    return ret;
}

} // end namespace pic

#endif

#endif /* PIC_ALGORITHMS_POISSON_IMAGE_EDITING_HPP */

