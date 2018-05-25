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
        img_str = "../data/input/yellow_flowers.png";
    }

    printf("Reading an HDR file...");

    pic::Image img;
    img.Read(img_str);

    printf("Is it valid? ");
    if(img.isValid()) {
        printf("OK\n");

        printf("Creating an image with bayering RGGB...");
        pic::Image *img_RGGB = pic::FilterMosaic::Execute(&img, NULL);
        printf("Ok\n");

        printf("Debayering the image...");
        pic::Image out(1, img.width, img.height, img.channels);
        pic::FilterDemosaic::Execute(img_RGGB, &out);
        printf("Ok\n");

        printf("Computing the difference image...");
        img -= out;
        img.applyFunction(fabsf);
        printf("Ok\n");

        printf("Writing results...");
        img_RGGB->Write("../data/output/img_mosaiced.png");
        out.Write("../data/output/img_demosaiced.png");
        img->Write("../data/output/img_demosaiced_difference.png");
        printf("Ok\n");


    } else {
        printf("No, the file is not valid!\n");
    }

    return 0;
}
