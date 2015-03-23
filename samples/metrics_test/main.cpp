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

#include <QCoreApplication>

//This means that we disable Eigen; some functionalities cannot be used.
//For example, estimating the camera response function
#define PIC_DISABLE_EIGEN

//This means that OpenGL acceleration layer is disabled
#define PIC_DISABLE_OPENGL

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    printf("Reading an LDR file...");

    pic::Image img;
    //loading values in [0,255]; note: no gamma removal or normalization in [0,1]
    img.Read("../data/input/singapore.png", pic::LT_NONE);

    printf("Ok\n");

    printf("Is it valid? ");
    if(img.isValid()) {
        printf("OK\n");

        printf("Filtering the input image (blurring)...");
        pic::Image *img_flt = pic::FilterGaussian2D::Execute(&img, NULL, 12.0f);
        printf("Ok\n");

        img_flt->Write("../img_flt.pfm");

        printf("Computing the SSIM index...");
        double ssim_index_value = pic::SSIMIndex(&img, img_flt, 0.01f, 0.03f, 1.5f, 255.0f, true);
        printf("Ok\n");

        printf("SSIM value: %f\n", ssim_index_value);

    } else {
        printf("No, the file is not valid!\n");
    }

    return 0;
}
