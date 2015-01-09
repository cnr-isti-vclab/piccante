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

                if((j + 1) < (width - 1)) {
                    if(mask[indI + 1]) {
                        n += 1.0f;
                        tL.push_back(Eigen::Triplet< double > (count, index[indI + 1], -1.0));
                    }
                }

                if((j - 1) > -1) {
                    if(mask[indI - 1]) {
                        n += 1.0f;
                        tL.push_back(Eigen::Triplet< double > (count, index[indI - 1], -1.0));
                    }
                }

                if((i + 1) < (height - 1)) {
                    if(mask[indI + width]) {
                        n += 1.0f;
                        tL.push_back(Eigen::Triplet< double > (count, index[indI + width], -1.0));
                    }
                }

                if((i - 1) > -1) {
                    if(mask[indI - width]) {
                        n += 1.0f;
                        tL.push_back(Eigen::Triplet< double > (count, index[indI - width], -1.0));
                    }
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

                    if((j + 1) < (width - 1)) {
                        if(!mask[indI + 1]) {
                            b[count] += (*target)(j + 1, i)[k];
                        }
                    }

                    if((j - 1) > -1) {
                        if(!mask[indI - 1]) {
                            b[count] += (*target)(j - 1, i)[k];
                        }
                    }                        

                    if((i + 1) < (height - 1)) {
                        if(!mask[indI + width]) {
                            b[count] += (*target)(j, i + 1)[k];
                        }
                    }

                    if((i - 1) > -1) {
                        if(!mask[indI - width]) {
                           b[count] += (*target)(j, i - 1)[k];
                        }
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

