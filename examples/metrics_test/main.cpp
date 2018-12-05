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

    //Image values are loaded and normalized in [0, 1];
    //NOTE: gamma removal or CRF linearization are NOT applied.
    pic::Image img0, img1;
    img0.Read(img0_str, pic::LT_NOR);

    if(!bCreate) {
        img1.Read(img1_str, pic::LT_NOR);
    }

    printf("Is it valid? ");
    if(img0.isValid()) {
        printf("OK\n");

        std::string name = pic::removeLocalPath(img0_str);
        name = pic::removeExtension(name);

        pic::Image *tmp;
        if(bCreate) {
            printf("Filtering the input image (blurring)...");
            tmp = pic::FilterGaussian2D::execute(&img0, NULL, 16.0f);
            printf("Ok\n");
            tmp->Write("../data/output/" + name + "_flt.hdr");
        } else {
            tmp = &img1;
        }

        float ssim_index;
        pic::SSIMIndex metric;
        pic::Image *ssim_map = metric.execute(Double(&img0, tmp), ssim_index, NULL);
        printf("Ok\n");

        if(ssim_map != NULL) {
            ssim_map->Write("../data/output/" + name + "_ssim_map.hdr");
        }

        printf("SSIM index: %3.3f\n", ssim_index);
        printf("MSE: %3.3f\n", pic::MSE(&img0, tmp, false));
        printf("RMSE: %3.3f\n", pic::RMSE(&img0, tmp));
        printf("PSNR: %3.3f\n", pic::PSNR(&img0, tmp));
        printf("MAE: %3.3f\n", pic::MAE(&img0, tmp, false));
        printf("Relative Error: %f\n", pic::RelativeError(&img0, tmp));

        pic::TMQI tmqi;
        img0 *= 255.0f;
        float Q, N, S;
        tmqi.execute(pic::Double(tmp, &img0), Q, N, S, NULL);
        printf("TMQI -- Q: %f N: %f S: %f\n", Q, N, S);
    } else {
        printf("No, the file is not valid!\n");
    }

    return 0;
}
