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
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    printf("Reading an HDR file...");

    pic::ImageRAW img;
    img.Read("../data/input/bottles.hdr");

    printf("Ok\n");

    printf("Is it valid? ");
    if(img.isValid()) {
        printf("OK\n");

        bool bSameSize = true;//the output image is going to be the sames size of the input one

        //setting up a 64,64 pixels translation matrix
        printf("\nTranslating the image of 64,64 pixels...");
        pic::Matrix3x3 h;
        h.SetTranslationMatrix(64.0f, 64.0f);

        pic::ImageRAW *imgOut_tra = pic::FilerWarp2D::Execute(&img, NULL, h, bSameSize);
        printf("Ok\n");

        printf("Writing the output...");
        bool bWritten = imgOut_tra->Write("../data/output/bottles_translated_64_64_pixels.bmp");

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }

        //setting up a 45 degree rotation matrix
        printf("\nRotating the image of 45 degrees...");
        h.SetRotationMatrix(pic::Deg2Rad(45.0f));

        pic::ImageRAW *imgOut_rot = pic::FilerWarp2D::Execute(&img, NULL, h, bSameSize);
        printf("Ok\n");

        printf("Writing the output...");
        bWritten = imgOut_rot->Write("../data/output/bottles_rotated_45_degrees.bmp");

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }

        //setting up a shear degree rotation matrix
        printf("\nApplying shear transform to the image...");
        h.SetShearMatrix(0.2f, 0.1f);        
        pic::ImageRAW *imgOut_sheared = pic::FilerWarp2D::Execute(&img, NULL, h, bSameSize);

        printf("Ok\n");

        printf("Writing the output...");
        bWritten = imgOut_sheared->Write("../data/output/bottles_shear_transform.bmp");

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }

        //setting up a scaling matrix
        printf("\nApplying a scaling transform to the image...");
        h.SetScaleMatrix(2.0f, 0.5f);
        pic::ImageRAW *imgOut_scaled = pic::FilerWarp2D::Execute(&img, NULL, h, bSameSize);

        printf("Ok\n");

        printf("Writing the output...");
        bWritten = imgOut_scaled->Write("../data/output/bottles_scaled.bmp");

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
