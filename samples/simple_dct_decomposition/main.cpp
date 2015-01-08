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

//This means that we disable Eigen; some functionalities cannot be used.
//For example, estimating the camera response function
#define PIC_DISABLE_EIGEN

//This means that OpenGL acceleration layer is disabled
#define PIC_DISABLE_OPENGL

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    printf("Reading an LDR file...");

    pic::Image img;
    img.Read("../data/input/singapore.png");

    printf("Ok\n");

    printf("Is it valid? ");
    if(img.isValid()) {
        printf("OK\n");

        printf("DCT transform...");
        pic::Image *img_dct = pic::FilterDCT2D::Transform(&img, NULL, 8);
        printf(" Ok\n");

        printf("Removing small coefficients...");
        for(int i = 0; i < img_dct->size(); i++) {
            if(fabsf(img_dct->data[i]) < 0.025f) {
                img_dct->data[i] = 0.0f;
            }
        }
        printf(" Ok\n");

        pic::Image *imgOut = pic::FilterDCT2D::Inverse(img_dct, NULL, 8);

        printf("Writing the file to disk...");
        bool bWritten = imgOut->Write("../data/output/simple_dct.png");

        if(bWritten) {
            printf(" Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }
    } else {
        printf("No, the file is not valid!\n");
    }

    return 0;
}
