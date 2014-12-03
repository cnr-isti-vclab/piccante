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

//This means that OpenGL acceleration layer is disabled
#define PIC_DISABLE_OPENGL

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    printf("Reading images...");

    pic::Image imgA, imgB, maskA;
    img_target.Read("../data/input/laplacian/target.png", pic::LT_NOR);
    img_source.Read("../data/input/laplacian/source.png", pic::LT_NOR);
    mask_target.Read("../data/input/laplacian/mask.png", pic::LT_NOR);

    printf("Ok\n");

    printf("Are images valid? ");
    if( imgA.isValid() && imgB.isValid() && maskA.isValid()) {
        printf("OK\n");

        pic::Image mask_source(mask_target.width, mask_target.height, mask_target.channels);
        mask_source.Assign(1.0f);
        mask_source.Sub(&mask_target);

        //Creating Laplacian pyramids
        pic::Pyramid pyr_target(&img_target, true, 4);
        pic::Pyramid pyr_source(&img_source, true, 4);

        //Creating Gaussian pyramids
        pic::Pyramid pyr_mask_target(&mask_target, false, 4);
        pic::Pyramid pyr_mask_source(&mask_source, false, 4);

        //Blending
        pyr_target.Mul(&pyr_mask_target);
        pyr_source.Mul(&pyr_mask_source);

        pyr_target.Add(&pyr_source);

        pic::Image *imgOut = pyr_target.Reconstruct();

        imgOut->Write("../data/output/laplacian_blending_result.png", pic::LT_NOR);

    } else {
        printf("All images are not valid!\n");
    }

    return 0;
}
