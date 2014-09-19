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

    printf("Reading an LDR file...");

    pic::ImageRAW img;
    img.Read("../data/input/bottles.hdr", pic::LT_NOR);

    printf("Ok\n");

    printf("Is it valid? ");
    if(img.isValid()) {
        printf("OK\n");

        //Adding black pixels to the image
        int n = img.nPixels() / 10;

        for(int i = 0; i < n; i++) {
            int x = rand() % img.width;
            int y = rand() % img.height;

            float *data = img(x, y);

            for(int i = 0; i < img.channels; i++) {
                data[i] = 0.0f;
            }
        }

        img.Write("../data/output/bottles_black_pixels_pp.hdr");

        //Recovering black pixels with push-pull
        pic::PushPull pp;

        pic::ImageRAW *imgOut = pp.Execute(&img, 0.0f);

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
