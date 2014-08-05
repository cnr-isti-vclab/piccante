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

    printf("Reading an HDR file...");

    pic::ImageRAW img;
    img.Read("../data/input/yellow_flowers.png");

    printf("Is it valid? ");
    if(img.isValid()) {
        printf("OK\n");

        printf("Creating an image with bayering RGGB...");
        pic::ImageRAW *imgRGGB = pic::FilterMosaic::Execute(&img, NULL);
        printf("Ok\n");

        printf("Debayering the image...");
        pic::ImageRAW out(1, img.width, img.height, 3);
        pic::Demosaic(imgRGGB, &out);
        printf("Ok\n");

        printf("Computing the difference image...");
        img.Sub(&out);
        img.ApplyFunction(fabsf);
        printf("Ok\n");

        printf("Writing results...");
        imgRGGB->Write("../data/output/yellow_flowers_mosaiced.png");
        out.Write("../data/output/yellow_flowers_demosaiced.png");
        img.Write("../data/output/yellow_flowers_demosaiced_difference.png");
        printf("Ok\n");


    } else {
        printf("No it is not a valid file!\n");
    }

    return 0;
}
