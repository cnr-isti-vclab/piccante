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
    std::string img_str;

    if(argc == 2) {
        img_str = argv[1];
    } else {
        img_str = "../data/input/bottles.hdr";
    }

    printf("Reading an HDR file...");

    pic::Image img;
    img.Read(img_str);

    printf("Ok\n");

    printf("Is it valid? ");
    if(img.isValid()) {
        printf("OK\n");

        //we estimate the best exposure for this HDR image
        float fstop = pic::findBestExposureHistogram(&img);

        printf("The best exposure value (histogram-based) is: %f f-stops\n", fstop);

        pic::FilterSimpleTMO fltSimpleTMO(2.2f, fstop);

        pic::Image *img_histo_tmo = fltSimpleTMO.Process(Single(&img), NULL);

        /*pic::LT_NOR implies that when we save the image
          we just convert it to 8-bit withou applying gamma.
          In this case, this is fine, because gamma was already applied
          in the pic::FilterSimpleTMO*/
        bool bWritten = img_histo_tmo->Write("../data/output/simple_exp_histo_tmo.bmp", pic::LT_NOR);

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }

        //
        //
        //

        //we estimate the best exposure for this HDR image
        fstop = pic::findBestExposureMean(&img);

        printf("The best exposure value (mean-based) is: %f f-stops\n", fstop);

        fltSimpleTMO.update(2.2f, fstop);

        pic::Image *img_mean_tmo = fltSimpleTMO.Process(Single(&img), NULL);

        /*pic::LT_NOR implies that when we save the image
          we just convert it to 8-bit withou applying gamma.
          In this case, this is fine, because gamma was already applied
          in the pic::FilterSimpleTMO*/
        bWritten = img_mean_tmo->Write("../data/output/simple_exp_mean_tmo.bmp", pic::LT_NOR);

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
