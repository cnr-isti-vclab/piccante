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

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    std::string img_str;

    if(argc == 3) {
        img_str = argv[1];
    } else {
        img_str = "../data/input/singapore.png";
    }

    printf("Reading an LDR file...");

    pic::Image img;
    img.Read(img_str);

    printf("Ok\n");

    printf("Is it valid? ");
    if(img.isValid()) {
        printf("OK\n");

        //from RGB to CIE Lab
        pic::Image *img_CIE_Lab = pic::FilterColorConv::fromRGBtoCIELAB(&img, NULL);

        //from CIE Lab to RGB
        pic::Image *img_RGB = pic::FilterColorConv::fromCIELABtoRGB2(img_CIE_Lab, NULL);

        printf("Writing the file to disk...");
        bool bWritten = img_CIE_Lab->Write("../data/output/singapore_CIE_Lab.pfm");
        bWritten = bWritten && img_RGB->Write("../data/output/singapore_RGB.png");

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
