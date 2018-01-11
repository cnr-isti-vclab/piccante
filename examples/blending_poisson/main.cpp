/*

PICCANTE Examples
The hottest examples of Piccante:
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3.0 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    See the GNU Lesser General Public License
    ( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.
*/

//This means that OpenGL acceleration layer is disabled
#define PIC_DISABLE_OPENGL

#include "../common_code/image_qimage_interop.hpp"

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    std::string img0_str, img1_str, img2_str;

    if(argc == 4) {
        img0_str = argv[1];
        img1_str = argv[2];
        img2_str = argv[3];
    } else {
        img0_str = "../data/input/poisson/target.png";
        img1_str = "../data/input/poisson/source.png";
        img2_str = "../data/input/poisson/mask.png";
    }

    printf("Reading images...");

    pic::Image img_target, img_source, mask_source;
    ImageRead(img0_str, &img_target, pic::LT_NOR);
    ImageRead(img1_str, &img_source, pic::LT_NOR);
    ImageRead(img2_str, &mask_source, pic::LT_NOR);

    printf("Ok\n");

    printf("Are images valid? ");
    if( img_target.isValid() && img_source.isValid() && mask_source.isValid()) {
        printf("OK\n");

        float color[] = {1.0f, 1.0f, 1.0f};
        bool *mask = mask_source.convertToMask(color, 0.1f, false);

        pic::Image *imgOut = pic::computePoissonImageEditing(&img_source, &img_target, mask);

        ImageWrite(imgOut, "../data/output/poisson_blending_result.png", pic::LT_NOR);
    } else {
        printf("Images are not valid!\n");
    }

    return 0;
}
