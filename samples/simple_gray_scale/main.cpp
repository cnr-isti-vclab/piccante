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
    img.Read("../data/input/singapore.png");

    printf("Ok\n");

    printf("Is it valid? ");
    if(img.isValid()) {
        printf("OK\n");

        bool bWritten;

        //Computing grey scale by computing the mean of color channels
        printf("Computing a gray scale image by computing the mean of color channels...");
        pic::Image *img_mean = pic::FilterLuminance::Execute(&img, NULL, pic::LT_MEAN);

        bWritten = img_mean->Write("../data/output/singapore_mean.png");

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }

        //Computing grey scale by computing the weighted average following CIE weights for Y
        printf("Computing a gray scale image by computing the weighted mean of color channels using CIE weights for Y...");
        pic::Image *img_cie_y = pic::FilterLuminance::Execute(&img, NULL, pic::LT_CIE_LUMINANCE);

        bWritten = img_cie_y->Write("../data/output/singapore_cie_y.png");

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }


        //Computing grey scale by using Exposure Fusion
        printf("Computing a gray scale image by using Exposure Fusion...");
        pic::Image *img_cg_ef = pic::ColorToGray(&img, NULL);

        bWritten = img_cg_ef->Write("../data/output/singapore_cg_ef.png");

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }

    } else {
        printf("No it is not a valid file!\n");
    }

    return 0;
}
