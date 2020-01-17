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

        printf("Tone mapping using global Reinhard et al.'s TMO...");
        image_tmo = pic::ReinhardTMO::executeGlobal1(&img, image_tmo);

        /*pic::LT_NOR_GAMMA implies that when we save the image,
          this is quantized at 8-bit and gamma is applied.
          Note that pic::ReinhardTMO tone maps an HDR image
          but it does not apply gamma.*/
        bWritten = image_tmo->Write("../data/output/" + nameOut + "_reinhard_gtmo1.png", pic::LT_NOR_GAMMA);

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }

        printf("Tone mapping using global Reinhard et al.'s TMO...");
        image_tmo = pic::ReinhardTMO::executeGlobal2(&img, image_tmo);

        /*pic::LT_NOR_GAMMA implies that when we save the image,
          this is quantized at 8-bit and gamma is applied.
          Note that pic::ReinhardTMO tone maps an HDR image
          but it does not apply gamma.*/
        bWritten = image_tmo->Write("../data/output/" + nameOut + "_reinhard_gtmo2.png", pic::LT_NOR_GAMMA);

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }

        printf("Tone mapping using local Reinhard et al.'s TMO...");
        image_tmo = pic::ReinhardTMO::executeLocal1(&img, image_tmo);

        /*pic::LT_NOR_GAMMA implies that when we save the image,
          this is quantized at 8-bit and gamma is applied.
          Note that pic::ReinhardTMO tone maps an HDR image
          but it does not apply gamma.*/
        bWritten = image_tmo->Write("../data/output/" + nameOut + "_reinhard_ltmo1.png", pic::LT_NOR_GAMMA);

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }

        printf("Tone mapping using local Reinhard et al.'s TMO...");
        image_tmo = pic::ReinhardTMO::executeLocal2(&img, image_tmo);

        /*pic::LT_NOR_GAMMA implies that when we save the image,
          this is quantized at 8-bit and gamma is applied.
          Note that pic::ReinhardTMO tone maps an HDR image
          but it does not apply gamma.*/
        bWritten = image_tmo->Write("../data/output/" + nameOut + "_reinhard_ltmo2.png", pic::LT_NOR_GAMMA);

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
          Note that pic::LischinskiTMO tone maps an HDR image
          but it does not apply gamma.*/
        bWritten = image_tmo->Write("../data/output/" + nameOut + "_lischinski_tmo.png", pic::LT_NOR_GAMMA);

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }

        printf("Tone mapping using Tumblin et al. 1999 TMO...");
        image_tmo = pic::TumblinTMO::execute(&img, image_tmo);

        /*pic::LT_NOR_GAMMA implies that when we save the image,
          this is quantized at 8-bit and gamma is applied.
          Note that pic::LischinskiTMO tone maps an HDR image
          but it does not apply gamma.*/
        bWritten = image_tmo->Write("../data/output/" + nameOut + "_tumblin_tmo.png", pic::LT_NOR_GAMMA);

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
