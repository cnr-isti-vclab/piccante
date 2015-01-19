/*

PICCANTE
The hottest HDR imaging library!
http://piccantelib.net

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

*/

//This means that OpenGL acceleration layer is disabled
#define PIC_DISABLE_OPENGL

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
