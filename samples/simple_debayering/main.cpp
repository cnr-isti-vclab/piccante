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
    img.Read("../data/input/yellow_flowers.png");

    printf("Is it valid? ");
    if(img.isValid()) {
        printf("OK\n");

        printf("Creating an image with bayering RGGB...");
        pic::Image *imgRGGB = pic::FilterMosaic::Execute(&img, NULL);
        printf("Ok\n");

        printf("Debayering the image...");
        pic::Image out(1, img.width, img.height, 3);
        pic::FilterDemosaic::Execute(imgRGGB, &out);
        printf("Ok\n");

        printf("Computing the difference image...");
        img -= out;
        img.ApplyFunction(fabsf);
        printf("Ok\n");

        printf("Writing results...");
        imgRGGB->Write("../data/output/yellow_flowers_mosaiced.png");
        out.Write("../data/output/yellow_flowers_demosaiced.png");
        img.Write("../data/output/yellow_flowers_demosaiced_difference.png");
        printf("Ok\n");


    } else {
        printf("No, the file is not valid!\n");
    }

    return 0;
}
