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

        //we estimate the best exposure for this HDR image
        float fstop = pic::FindBestExposure(&img);

        printf("The best exposure value is: %f f-stops\n", fstop);

        pic::FilterSimpleTMO fltSimpleTMO(2.2f, fstop);

        pic::Image *imgToneMapped = fltSimpleTMO.Process(Single(&img), NULL);

        /*pic::LT_NOR implies that when we save the image
          we just convert it to 8-bit withou applying gamma.
          In this case, this is fine, because gamma was already applied
          in the pic::FilterSimpleTMO*/
        bool bWritten = imgToneMapped->Write("../data/output/simple_tmo.bmp", pic::LT_NOR);

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
