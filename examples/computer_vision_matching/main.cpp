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

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    std::string img0_str, img1_str;

    if(argc == 3) {
        img0_str = argv[1];
        img1_str = argv[2];
    } else {
        img0_str = "../data/input/features/balcony_0.png";
        img1_str = "../data/input/features/balcony_1.png";
    }

    pic::Image img0, img1;
    img0.Read(img0_str, pic::LT_NOR);
    img1.Read(img1_str, pic::LT_NOR);

    printf("Ok\n");

    printf("Are they both valid? ");
    if(img0.isValid() && img1.isValid()) {
        printf("OK\n");

        auto H = getHomographyMatrixFromTwoImage(&img0, &img1);

        printf("\nHomography matrix:\n");
        pic::printfMat(H);

        printf("Applying H to the first image..");
        pic::Image *img0_H = pic::FilterWarp2D::execute(&img0, NULL, pic::MatrixConvert(H), true, false);
        img0_H->Write("../data/output/simple_matching_img_0_H_applied.png", pic::LT_NOR);
        printf("Ok.\n");
    } else {
        printf("No there is at least an invalid file!\n");
    }

    return 0;
}
