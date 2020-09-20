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
        pic::Image *img_RGGB = pic::FilterMosaic::execute(&img, NULL);
        printf("Ok\n");

        printf("Debayering the image...");
        pic::Image out(1, img.width, img.height, img.channels);
        pic::FilterDemosaic::execute(img_RGGB, &out);
        printf("Ok\n");

        printf("Computing the difference image...");
        img -= out;
        img.applyFunction(fabsf);
        printf("Ok\n");

        printf("Writing results...");
        img_RGGB->Write("../data/output/img_mosaiced.png");
        out.Write("../data/output/img_demosaiced.png");
        img.Write("../data/output/img_demosaiced_difference.png");
        printf("Ok\n");

    } else {
        printf("No, the file is not valid!\n");
    }

    return 0;
}
