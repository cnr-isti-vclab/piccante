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

        pic::ImageSamplerLanczos is_lan;
        out = pic::FilterSampler2D::Execute(&img, out, 2.0f, &is_lan);

        if(out != NULL) {
            out->Write("../data/output/bottles_2x_lanczos.hdr");
        }
    } else {
        printf("No, the file is not valid!\n");
    }

    return 0;
}
