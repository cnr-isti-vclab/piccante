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
    std::string img_str;

    if(argc == 2) {
        img_str = argv[1];
    } else {
        img_str = "../data/input/bottles.hdr";
    }

    printf("Reading an HDR file...");

    pic::Image img;
    img.Read(img_str);

    printf("Ok\n");

    printf("Is it valid? ");
    if(img.isValid()) {
        printf("OK\n");

        std::string nameOut = pic::getFileNameOnly(img_str);

        bool bWritten;
        pic::Image *image_tmo = NULL;

        printf("Tone mapping using Schlick 1994 TMO...");
        image_tmo = pic::SchlickTMO::execute(&img, image_tmo);
        /*pic::LT_NOR_GAMMA implies that when we save the image,
          this is quantized at 8-bit and gamma is applied.
          Note that pic::Schlick tone maps an HDR image
          but it does not apply gamma.*/
        bWritten = image_tmo->Write("../data/output/" + nameOut + "_schlick_tmo.png", pic::LT_NOR_GAMMA);


        printf("Tone mapping using Ferwerda et al. 1996 TMO...");
        image_tmo = pic::FerwerdaTMO::execute(&img, image_tmo);
        /*pic::LT_NOR_GAMMA implies that when we save the image,
          this is quantized at 8-bit and gamma is applied.
          Note that pic::FerwerdaTMO tone maps an HDR image
          but it does not apply gamma.*/
        bWritten = image_tmo->Write("../data/output/" + nameOut + "_ferwerda_tmo.png", pic::LT_NOR_GAMMA);

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }

        printf("Tone mapping using Reinhard et al.'s TMO...");
        image_tmo = pic::ReinhardTMO::execute(&img, image_tmo);

        /*pic::LT_NOR_GAMMA implies that when we save the image,
          this is quantized at 8-bit and gamma is applied.
          Note that pic::ReinhardTMO tone maps an HDR image
          but it does not apply gamma.*/
        bWritten = image_tmo->Write("../data/output/" + nameOut + "_reinhard_tmo.png", pic::LT_NOR_GAMMA);

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }

        printf("Tone mapping using Durand and Dorsey's TMO...");
        image_tmo = pic::DurandTMO::execute(&img, image_tmo);

        /*pic::LT_NOR_GAMMA implies that when we save the image,
          this is quantized at 8-bit and gamma is applied.
          Note that pic::DurandTMO tone maps an HDR image
          but it does not apply gamma.*/
        bWritten = image_tmo->Write("../data/output/" + nameOut + "_durand_tmo.png", pic::LT_NOR_GAMMA);

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }


        printf("Tone mapping using Drago et al.'s TMO...");
        image_tmo = pic::DragoTMO::execute(&img, image_tmo);

        /*pic::LT_NOR_GAMMA implies that when we save the image,
          this is quantized at 8-bit and gamma is applied.
          Note that pic::DragoTMO tone maps an HDR image
          but it does not apply gamma.*/
        bWritten = image_tmo->Write("../data/output/" + nameOut + "_drago_tmo.png", pic::LT_NOR_GAMMA);

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }

        printf("Tone mapping using Ward Histogram Adjustment TMO...");
        image_tmo = pic::WardHistogramTMO::execute(&img, image_tmo);

        /*pic::LT_NOR_GAMMA implies that when we save the image,
          this is quantized at 8-bit and gamma is applied.
          Note that pic::WardTMO tone maps an HDR image
          but it does not apply gamma.*/
        bWritten = image_tmo->Write("../data/output/" + nameOut + "_ward_tmo.png", pic::LT_NOR_GAMMA);

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }

        printf("Tone mapping using Lischinski et al. 2006 automatic TMO...");
        image_tmo = pic::LischinskiTMO::execute(&img, image_tmo);

        /*pic::LT_NOR_GAMMA implies that when we save the image,
          this is quantized at 8-bit and gamma is applied.
          Note that pic::WardTMO tone maps an HDR image
          but it does not apply gamma.*/
        bWritten = image_tmo->Write("../data/output/" + nameOut + "_lischinski_tmo.png", pic::LT_NOR_GAMMA);

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }

    } else {
        printf("No, the file is not valid!\n");
    }

    return 0;
}
