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
        printf("OK\n");

        printf("Tone mapping using Reinhard et al.'s TMO...");
        pic::ImageRAW *imgToneMapped_reinhard = pic::ReinhardTMO(&img);

        /*pic::LT_NOR_GAMMA implies that when we save the image,
          this is quantized at 8-bit and gamma is applied.
          Note that pic::ReinhardTMO tone maps an HDR image
          but it does not apply gamma.*/
        bool bWritten = imgToneMapped_reinhard->Write("../data/output/reinhard_tmo.png", pic::LT_NOR_GAMMA);

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }

        printf("Tone mapping using Drago et al.'s TMO...");
        pic::ImageRAW *imgToneMapped_drago = pic::DragoTMO(&img);

        /*pic::LT_NOR_GAMMA implies that when we save the image,
          this is quantized at 8-bit and gamma is applied.
          Note that pic::DragoTMO tone maps an HDR image
          but it does not apply gamma.*/
        bWritten = imgToneMapped_drago->Write("../data/output/drago_tmo.png", pic::LT_NOR_GAMMA);

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }


        printf("Tone mapping using Ward Histogram Adjustment TMO...");
        pic::ImageRAW *imgToneMapped_ward = pic::WardHistogramTMO(&img);

        /*pic::LT_NOR_GAMMA implies that when we save the image,
          this is quantized at 8-bit and gamma is applied.
          Note that pic::WardTMO tone maps an HDR image
          but it does not apply gamma.*/
        bWritten = imgToneMapped_ward->Write("../data/output/ward_tmo.png", pic::LT_NOR_GAMMA);

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }

        printf("Tone mapping using Lischinski et al. 2006 automatic TMO...");
        pic::ImageRAW *imgToneMapped_lischinski = pic::LischinskiTMO(&img, NULL, 0.5f);

        /*pic::LT_NOR_GAMMA implies that when we save the image,
          this is quantized at 8-bit and gamma is applied.
          Note that pic::WardTMO tone maps an HDR image
          but it does not apply gamma.*/
        bWritten = imgToneMapped_lischinski->Write("../data/output/lischinski_tmo.png", pic::LT_NOR_GAMMA);

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
