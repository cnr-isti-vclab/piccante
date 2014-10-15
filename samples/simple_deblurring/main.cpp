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

//This means that OpenGL acceleration layer is disabled
#define PIC_DISABLE_OPENGL

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    printf("Reading an HDR file...");

    pic::ImageRAW img;
    img.Read("../data/input/bottles.hdr");

    printf("Ok\n");

    printf("Is it valid? ");
    if(img.isValid()) {
        printf("Ok\n");

        printf("Filtering the image with a PSF read from file...");

        pic::ImageRAW psf("../data/input/kernel_psf.png");

        //normalization of the PSF
        float *norm = psf.getSumVal(NULL, NULL);
        psf.Div(norm[0]);

        pic::ImageRAW *conv = pic::FilterConv2D::Execute(&img, &psf, NULL);
        conv->Write("../data/output/bottles_conv_kernel_psf.hdr");

        printf("Ok!\n");

        printf("Filtering the image with a PSF read from file...");
        pic::ImageRAW *deconv = pic::RichardsonLucyDeconvolution(conv, &psf, 1000, NULL);

        printf("Ok!\n");

        printf("Writing the file to disk...");
        bool bWritten = deconv->Write("../data/output/bottles_deconv_kernel_psf.hdr");

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
