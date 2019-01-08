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

        bool bWritten;

        printf("Tone mapping using Raman et al. 2009 TMO...");
        pic::Image *img_tmo_raman = pic::RamanTMO::execute(&img, NULL);
        /*pic::LT_NOR_GAMMA implies that when we save the image,
          this is quantized at 8-bit and gamma is applied.
          Note that pic::Schlick tone maps an HDR image
          but it does not apply gamma.*/
        bWritten = img_tmo_raman->Write("../data/output/img_tmo_raman.png", pic::LT_NOR_GAMMA);


        printf("Tone mapping using Schlick 1994 TMO...");
        pic::Image *img_tmo_schlick = pic::SchlickTMO::execute(&img, NULL);
        /*pic::LT_NOR_GAMMA implies that when we save the image,
          this is quantized at 8-bit and gamma is applied.
          Note that pic::Schlick tone maps an HDR image
          but it does not apply gamma.*/
        bWritten = img_tmo_schlick->Write("../data/output/img_tmo_schlick.png", pic::LT_NOR_GAMMA);


        printf("Tone mapping using Ferwerda et al. 1996 TMO...");
        pic::Image *img_tmo_ferwerda = pic::FerwerdaTMO::execute(&img, NULL);
        /*pic::LT_NOR_GAMMA implies that when we save the image,
          this is quantized at 8-bit and gamma is applied.
          Note that pic::FerwerdaTMO tone maps an HDR image
          but it does not apply gamma.*/
        bWritten = img_tmo_ferwerda->Write("../data/output/img_tmo_ferwerda.png", pic::LT_NOR_GAMMA);

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }

        printf("Tone mapping using Reinhard et al.'s TMO...");
        pic::Image *img_tmo_reinhard = pic::ReinhardTMO::execute(&img, NULL);

        /*pic::LT_NOR_GAMMA implies that when we save the image,
          this is quantized at 8-bit and gamma is applied.
          Note that pic::ReinhardTMO tone maps an HDR image
          but it does not apply gamma.*/
        bWritten = img_tmo_reinhard->Write("../data/output/img_tmo_reinhard.png", pic::LT_NOR_GAMMA);

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }

        printf("Tone mapping using Durand and Dorsey's TMO...");
        pic::Image *img_tmo_durand = pic::DurandTMO::execute(&img, NULL);

        /*pic::LT_NOR_GAMMA implies that when we save the image,
          this is quantized at 8-bit and gamma is applied.
          Note that pic::DurandTMO tone maps an HDR image
          but it does not apply gamma.*/
        bWritten = img_tmo_durand->Write("../data/output/img_tmo_durand.png", pic::LT_NOR_GAMMA);

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }


        printf("Tone mapping using Drago et al.'s TMO...");
        pic::Image *img_tmo_drago = pic::DragoTMO::execute(&img, NULL);

        /*pic::LT_NOR_GAMMA implies that when we save the image,
          this is quantized at 8-bit and gamma is applied.
          Note that pic::DragoTMO tone maps an HDR image
          but it does not apply gamma.*/
        bWritten = img_tmo_drago->Write("../data/output/img_tmo_drago.png", pic::LT_NOR_GAMMA);

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }

        printf("Tone mapping using Ward Histogram Adjustment TMO...");
        pic::Image *img_tmo_ward = pic::WardHistogramTMO::execute(&img, NULL);

        /*pic::LT_NOR_GAMMA implies that when we save the image,
          this is quantized at 8-bit and gamma is applied.
          Note that pic::WardTMO tone maps an HDR image
          but it does not apply gamma.*/
        bWritten = img_tmo_ward->Write("../data/output/img_tmo_ward.png", pic::LT_NOR_GAMMA);

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }

        printf("Tone mapping using Lischinski et al. 2006 automatic TMO...");
        pic::Image *img_tmo_lischinski = pic::LischinskiTMO::execute(&img, NULL);

        /*pic::LT_NOR_GAMMA implies that when we save the image,
          this is quantized at 8-bit and gamma is applied.
          Note that pic::WardTMO tone maps an HDR image
          but it does not apply gamma.*/
        bWritten = img_tmo_lischinski->Write("../data/output/img_tmo_lischinski.png", pic::LT_NOR_GAMMA);

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
