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

//This means that we disable Eigen; some functionalities cannot be used.
//For example, estimating the camera response function
#define PIC_DISABLE_EIGEN

//This means that OpenGL acceleration layer is disabled
#define PIC_DISABLE_OPENGL

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    std::string img0_str, img1_str;
    bool bCreate = false;

    if(argc == 3) {
        img0_str = argv[1];
        img1_str = argv[2];
    } else {
        img0_str = "../data/input/singapore.png";
        bCreate = true;
    }

    //load  values in [0, 255]; note: no gamma removal or normalization in [0, 1]
    pic::Image img0, img1;
    img0.Read(img0_str, pic::LT_NOR);

    if(!bCreate) {
        img1.Read(img1_str, pic::LT_NOR);
    }

    printf("Is it valid? ");
    if(img0.isValid()) {
        printf("OK\n");

        pic::Image *tmp;
        if(bCreate) {
            printf("Filtering the input image (blurring)...");
            tmp = pic::FilterGaussian2D::Execute(&img0, NULL, 12.0f);
            printf("Ok\n");
        }

        tmp->Write("../data/output/metrics_img_flt.hdr");

        printf("Computing the SSIM index...");
        float ssim_index;

        pic::Image *ssim_map = pic::SSIMIndex(&img0, tmp, ssim_index, NULL, 0.01f, 0.03f, 1.5f, 255.0f);
        printf("Ok\n");

        if(ssim_map != NULL) {
            ssim_map->Write("../data/output/metrics_ssim_map.hdr");
        }

        printf("SSIM index: %f\n", ssim_index);

    } else {
        printf("No, the file is not valid!\n");
    }

    return 0;
}
