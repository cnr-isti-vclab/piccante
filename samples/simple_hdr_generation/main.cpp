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

    printf("Reading a stack of LDR images...");

    pic::Image img[3];
    img[0].Read("../data/input/stack_alignment/IMG_4209.jpg", pic::LT_NOR);
    img[1].Read("../data/input/stack_alignment/IMG_4210.jpg", pic::LT_NOR);
    img[2].Read("../data/input/stack_alignment/IMG_4211.jpg", pic::LT_NOR);

    //exposure time in seconds: exposureTime[0] ==> img[0], etc.
    float exposureTime[] = {1.0f, 0.25f, 4.0f};

    printf("Ok\n");

    printf("Are these images valid? ");
    if(img[0].isValid() && img[1].isValid() && img[2].isValid()) {
        printf("Ok\n");

        printf("We now align bright and dark exposure images to the well-exposed one... ");
        Eigen::Vector2i shift_dark;
        pic::Image *img_dark = pic::WardAlignment::Execute(&img[0], &img[1], shift_dark);
        img_dark->Write("../data/output/stack_aligned_dark.jpg", pic::LT_NOR);

        Eigen::Vector2i shift_bright;
        pic::Image *img_bright = pic::WardAlignment::Execute(&img[0], &img[2], shift_bright);
        img_bright->Write("../data/output/stack_aligned_bright.jpg", pic::LT_NOR);
        printf("Ok\n");

        //Estimating the camera response function
        printf("Estimating the camera response function... ");
        pic::ImageVec stack = Triple(&img[0], img_dark, img_bright);

        pic::CameraResponseFunction crf;
        crf.DebevecMalik(stack, exposureTime);
        printf("Ok.\n");

        //Setting each exposure time to the related image
        for(int i=0; i<3; i++) {            
            stack[i]->exposure = exposureTime[i];
        }

        printf("Assembling the different exposure images... ");
        pic::FilterAssembleHDR fltAHDR(pic::CRF_GAUSS, pic::LIN_ICFR, &crf.icrf);
        pic::Image *imgOut = fltAHDR.ProcessP(stack, NULL);
        printf("Ok\n");

        if(imgOut != NULL) {
            imgOut->Write("../data/output/simple_hdr_assembled.hdr");
        }


    } else {
        printf("No, the files are not valid!\n");
    }

    return 0;
}
