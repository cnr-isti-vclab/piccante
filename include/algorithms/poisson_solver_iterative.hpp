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

#ifndef PIC_ALGORITHMS_ITERATIVE_POISSON_SOLVER_HPP
#define PIC_ALGORITHMS_ITERATIVE_POISSON_SOLVER_HPP

#include "image.hpp"

namespace pic {

/**
 * @brief PoissonSolverIterative
 * @param img
 * @param laplacian
 * @param coords
 * @param maxSteps
 * @return
 */
Image *PoissonSolverIterative(Image *img, Image *laplacian,
                              std::vector<int> coords,
                              int maxSteps = 100)
{
    #ifdef PIC_DEBUG
        printf("Iterative Poisson solver... ");
    #endif

    if(maxSteps < 1) {
        maxSteps = 20000;
    }

    Image *tmpImg = img->Clone();
    Image *tmpSwap = NULL;

    int c, coord, x, y;
    float workValue;

    for(int i = 0; i < maxSteps; i++) {
        for(unsigned int j = 0; j < coords.size(); j++) {
            coord = coords[j];
            img->ReverseAddress(coord, x, y);

            workValue = -laplacian->data[coord];

            c = img->Address(x + 1, y);
            workValue += img->data[c];

            c = img->Address(x - 1, y);
            workValue += img->data[c];

            c = img->Address(x, y + 1);
            workValue += img->data[c];

            c = img->Address(x, y - 1);
            workValue += img->data[c];

            tmpImg->data[coord] = workValue / 4.0f;
        }

        tmpSwap = img;
        img     = tmpImg;
        tmpImg  = tmpSwap;
    }

    #ifdef PIC_DEBUG
        printf("done.\n");
    #endif

    return img;
}

} // end namespace pic

#endif /* PIC_ALGORITHMS_ITERATIVE_POISSON_SOLVER_HPP */

