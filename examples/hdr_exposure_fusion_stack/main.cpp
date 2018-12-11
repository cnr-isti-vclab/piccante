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

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    printf("Reading a stack of LDR images...");
    //reading images and storing them with normalized values in [0,1]
    pic::Image img[3];
    img[0].Read("../data/input/stack_alignment/IMG_4209.jpg", pic::LT_NOR);
    img[1].Read("../data/input/stack_alignment/IMG_4210.jpg", pic::LT_NOR);
    img[2].Read("../data/input/stack_alignment/IMG_4211.jpg", pic::LT_NOR);

    printf("Ok\n");

    printf("Are these images valid? ");
    if(img[0].isValid() && img[1].isValid() && img[2].isValid()) {
        printf("Ok\n");

        printf("Aligning bright and dark exposure images to the well-exposed one... ");
        pic::Vec2i shift_dark;
        pic::Image *img_dark = pic::WardAlignment::execute(&img[0], &img[1], shift_dark);
        img_dark->Write("../data/output/stack_aligned_dark.png", pic::LT_NOR);

        pic::Vec2i shift_bright;
        pic::Image *img_bright = pic::WardAlignment::execute(&img[0], &img[2], shift_bright);
        img_bright->Write("../data/output/stack_aligned_bright.png", pic::LT_NOR);
        printf("Ok\n");

        printf("Fusing the aligned images... ");
        auto image_vec = pic::Triple(&img[0], img_bright, img_dark);
        pic::ExposureFusion ef(0.2f, 1.0f, 0.2f);
        pic::Image *imgOut = ef.ProcessStack(image_vec, NULL);

        printf("Ok\n");

        if(imgOut != NULL) {
            imgOut->Write("../data/output/stack_aligned.png", pic::LT_NOR);
        }

    } else {
        printf("No, the files are not valid!\n");
    }

    return 0;
}
