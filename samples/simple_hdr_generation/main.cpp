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

    printf("Reading a stack of LDR images...");

    pic::Image img[3];
    img[0].Read("../data/input/stack_alignment/IMG_4209.jpg", pic::LT_NOR);
    img[1].Read("../data/input/stack_alignment/IMG_4210.jpg", pic::LT_NOR);
    img[2].Read("../data/input/stack_alignment/IMG_4211.jpg", pic::LT_NOR);

    //exposure time in seconds: exposureTime[0] ==> img[0], etc.
    float exposureTime[] = {1.0f, 0.25f, 4.0f};

    printf("Ok\n");

    printf("Is it valid? ");
    if(img[0].isValid() && img[1].isValid() && img[2].isValid()) {
        printf("Ok\n");

        printf("We now align bright and dark exposure images to the well-exposed one...\n");
        Eigen::Vector2i shift_dark;
        pic::Image *img_dark = pic::WardAlignment::Execute(&img[0], &img[1], shift_dark);
        img_dark->Write("../data/output/stack_aligned_dark.jpg", pic::LT_NOR);

        Eigen::Vector2i shift_bright;
        pic::Image *img_bright = pic::WardAlignment::Execute(&img[0], &img[2], shift_bright);
        img_bright->Write("../data/output/stack_aligned_bright.jpg", pic::LT_NOR);
        printf("\nOk\n");

        //Estimating the camera response function
        printf("Estimating the camera response function...\n");
        pic::ImageVec stack = Triple(&img[0], img_dark, img_bright);

        pic::CameraResponseFunction crf;
        crf.DebevecMalik(stack, exposureTime);
        printf("\nOk.\n");

        //Setting each exposure time to the related image
        for(int i=0; i<3; i++) {            
            stack[i]->exposure = exposureTime[i];
        }

        printf("Assembling the different exposure images...");
        pic::FilterAssembleHDR fltAHDR(pic::CRF_GAUSS, pic::LIN_ICFR, &crf.icrf);
        pic::Image *imgOut = fltAHDR.ProcessP(stack, NULL);
        printf("Ok\n");

        if(imgOut != NULL) {
            imgOut->Write("../data/output/simple_hdr_assembled.hdr");
        }


    } else {
        printf("No it is not a valid file!\n");
    }

    return 0;
}
