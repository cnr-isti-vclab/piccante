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

//This means that we disable Eigen; some functionalities cannot be used.
//For example, estimating the camera response function
#define PIC_DISABLE_EIGEN

//This means that OpenGL acceleration layer is disabled
#define PIC_DISABLE_OPENGL

//This means we do not use QT for I/O
#define PIC_DISABLE_QT

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    printf("Reading an HDR file...");

    pic::Image img;
    img.Read("../data/input/bottles.hdr");

    printf("Ok\n");

    printf("Is it valid? ");
    if(img.isValid()) {
        printf("OK\n");

        pic::ImageSamplerGaussian is_lc;
        pic::Image *out_d = pic::FilterDownSampler2D::Execute(&img, NULL, 0.5f);
        if(out_d != NULL) {
            out_d->Write("../data/output/bottles_half_gaussian.hdr");
        }

        pic::ImageSamplerNearest is_near;
        pic::Image *out = pic::FilterSampler2D::Execute(&img, NULL, 2.0f, &is_near);

        if(out != NULL) {
            out->Write("../data/output/bottles_2x_nearest.hdr");
        }

        pic::ImageSamplerBilinear is_bil;
        out = pic::FilterSampler2D::Execute(&img, out, 2.0f, &is_bil);

        if(out != NULL) {
            out->Write("../data/output/bottles_2x_bilinear.hdr");
        }

        pic::ImageSamplerCatmullRom is_cr;
        out = pic::FilterSampler2D::Execute(&img, out, 2.0f, &is_cr);

        if(out != NULL) {
            out->Write("../data/output/bottles_2x_catmull_rom.hdr");
        }

        pic::ImageSamplerBicubic is_bic;
        out = pic::FilterSampler2D::Execute(&img, out, 2.0f, &is_bic);

        if(out != NULL) {
            out->Write("../data/output/bottles_2x_bicubic.hdr");
        }
    } else {
        printf("No, the file is not valid!\n");
    }

    return 0;
}
