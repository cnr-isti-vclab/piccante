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

#define PIC_DEBUG

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    printf("Reading images...");

    pic::Image img_target, img_source, mask_source;
    img_target.Read("../data/input/poisson/target.png", pic::LT_NOR);
    img_source.Read("../data/input/poisson/source.png", pic::LT_NOR);
    mask_source.Read("../data/input/poisson/mask.png", pic::LT_NOR);

    printf("Ok\n");

    printf("Are images valid? ");
    if( img_target.isValid() && img_source.isValid() && mask_source.isValid()) {
        printf("OK\n");

        float color[] = {1.0f, 1.0f, 1.0f};
        bool *mask = mask_source.ConvertToMask(color, 0.1f, false);

        pic::Image *imgOut = PoissonImageEditing(&img_source, &img_target, mask);
        imgOut->Write("../data/output/poisson_blending_result.png", pic::LT_NOR);


    } else {
        printf("Images are not valid!\n");
    }

    return 0;
}
