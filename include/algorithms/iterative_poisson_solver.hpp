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

#ifndef PIC_ALGORITHMS_ITERATIVE_POISSON_SOLVER_HPP
#define PIC_ALGORITHMS_ITERATIVE_POISSON_SOLVER_HPP

#include "image_raw.hpp"

namespace pic {

ImageRAW *IterativePoissonSolver(ImageRAW *img,
                                 ImageRAW *laplacian,
                                 std::vector<int> coords,
                                 int maxSteps = 100)
{
    //Iterative Poisson solver
    printf("Iterative Poisson solver... ");

    if(maxSteps < 1) {
        maxSteps = 20000;
    }

    ImageRAW *tmpImg = img->Clone();
    ImageRAW *tmpSwap = NULL;

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

    printf("done.\n");
    return img;
}

} // end namespace pic

#endif /* PIC_ALGORITHMS_ITERATIVE_POISSON_SOLVER_HPP */

