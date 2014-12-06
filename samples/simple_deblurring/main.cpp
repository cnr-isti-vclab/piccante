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

//This means that OpenGL acceleration layer is disabled
#define PIC_DISABLE_OPENGL

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    printf("Reading an HDR file...");

    pic::Image img;
    img.Read("../data/input/bottles.hdr");

    printf("Ok\n");

    printf("Is it valid? ");
    if(img.isValid()) {
        printf("Ok\n");

        printf("Filtering the image with a PSF read from file...");
        pic::Image psf("../data/input/kernel_psf.png");

        //normalization of the PSF
        float *norm = psf.getSumVal(NULL, NULL);
        psf.Div(norm[0]);

        pic::Image *conv = pic::FilterConv2D::Execute(&img, &psf, NULL);
        conv->Write("../data/output/bottles_conv_kernel_psf.hdr");

        printf("Ok!\n");

        printf("Filtering the image with a PSF read from file...");
        pic::Image *deconv = pic::RichardsonLucyDeconvolution(conv, &psf, 1000, NULL);

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
