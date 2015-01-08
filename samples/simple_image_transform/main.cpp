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

        bool bSameSize = true;//the output image is going to be the sames size of the input one

        //setting up a 64,64 pixels translation matrix
        printf("\nTranslating the image of 64,64 pixels...");
        pic::Matrix3x3 h;
        h.SetTranslationMatrix(64.0f, 64.0f);

        pic::Image *imgOut_tra = pic::FilterWarp2D::Execute(&img, NULL, h, bSameSize);
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

        pic::Image *imgOut_rot = pic::FilterWarp2D::Execute(&img, NULL, h, bSameSize, true);
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
        pic::Image *imgOut_sheared = pic::FilterWarp2D::Execute(&img, NULL, h, bSameSize, true);

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
        pic::Image *imgOut_scaled = pic::FilterWarp2D::Execute(&img, NULL, h, bSameSize, true);

        printf("Ok\n");

        printf("Writing the output...");
        bWritten = imgOut_scaled->Write("../data/output/bottles_scaled.bmp");

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
