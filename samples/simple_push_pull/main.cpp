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

    printf("Reading an HDR file...");

    pic::Image img;
    img.Read("../data/input/bottles.hdr", pic::LT_NOR);

    printf("Ok\n");

    printf("Is it valid? ");
    if(img.isValid()) {
        printf("OK\n");

        pic::Image img_black(1, 32, 32, 3);
        img_black.SetZero();

        //Adding a hole in the image
        img.CopySubImage(&img_black, 292, 130);

        img.Write("../data/output/bottles_black_pixels_pp.hdr");

        //Recovering black pixels with push-pull
        pic::PushPull pp;

        pic::Image *imgOut = pp.Execute(&img, 0.0f);

        printf("Writing recovered result using Push-Pull... ");

        bool bWritten = imgOut->Write("../data/output/bottles_rec_pp.hdr");

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
