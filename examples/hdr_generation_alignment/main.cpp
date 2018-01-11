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

//This means that OpenGL acceleration layer is disabled
#define PIC_DISABLE_OPENGL

#include "../common_code/image_qimage_interop.hpp"

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    printf("Reading a stack of LDR images...");
    //reading images and storing them with normalized values in [0,1]
    pic::Image img[3];
    ImageRead("../data/input/stack_alignment/IMG_4209.jpg", &img[0], pic::LT_NOR);
    ImageRead("../data/input/stack_alignment/IMG_4210.jpg", &img[1], pic::LT_NOR);
    ImageRead("../data/input/stack_alignment/IMG_4211.jpg", &img[2], pic::LT_NOR);

    printf("Ok\n");

    printf("Are these images valid? ");
    if(img[0].isValid() && img[1].isValid() && img[2].isValid()) {
        printf("Ok\n");

        printf("Aligning bright and dark exposure images to the well-exposed one... ");
        Eigen::Vector2i shift_dark;
        pic::Image *img_dark = pic::WardAlignment::execute(&img[0], &img[1], shift_dark);
        ImageWrite(img_dark, "../data/output/stack_aligned_dark.jpg", pic::LT_NOR);

        Eigen::Vector2i shift_bright;
        pic::Image *img_bright = pic::WardAlignment::execute(&img[0], &img[2], shift_bright);
        ImageWrite(img_bright, "../data/output/stack_aligned_bright.jpg", pic::LT_NOR);
        printf("Ok\n");

        printf("Estimating the camera response function... ");

        //exposure time in seconds: exposureTime[0] ==> img[0], etc.
        float exposureTime[] = {1.0f, 0.25f, 4.0f};

        pic::ImageVec stack_crf = Triple(&img[0], &img[1], &img[2]);
        for(int i=0; i<3; i++) {
            stack_crf[i]->exposure = exposureTime[i];
        }

        pic::CameraResponseFunction crf;
        crf.DebevecMalik(stack_crf);
        printf("Ok.\n");

        //Set each exposure time to the related image
        pic::ImageVec stack = Triple(&img[0], img_dark, img_bright);
        for(int i=0; i<3; i++) {
            stack[i]->exposure = exposureTime[i];
        }

        printf("Assembling the different exposure images... ");
        pic::FilterAssembleHDR merger(&crf, pic::CW_DEB97, pic::HRD_LOG);
        pic::Image *imgOut = merger.ProcessP(stack, NULL);

        printf("Ok\n");

        if(imgOut != NULL) {
            ImageWrite(imgOut, "../data/output/stack_aligned_hdr.hdr");
        }

    } else {
        printf("No, the files are not valid!\n");
    }

    return 0;
}
