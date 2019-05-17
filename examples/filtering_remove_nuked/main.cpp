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

//This means that OpenGL acceleration layer is disabled
#define PIC_DISABLE_OPENGL

//This means we do not use QT for I/O
#define PIC_DISABLE_QT

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    if(argc == 2) {
        img0_str = argv[1];
    } else {
        img0_str = "../data/input/cornellbox_mat_pt.pfm";
    }
    printf("Reading an HDR file...");

    pic::Image img;
    img.Read(img0_str);

    printf("Ok\n");

    printf("Is it valid? ");
    if(img.isValid()) {
        printf("Ok\n");

        printf("Filtering the image with a Gaussian filter with sigma_s = 4.0...");

        pic::Image *output = pic::FilterRemoveInfNaN::Execute(&img, NULL);
        pic::Image *output2 = pic::FilterRemoveNuked::Execute(output, NULL, 0.95f);

        printf("Ok!\n");

        printf("Writing the file to disk...");
        bool bWritten = output2->Write("../data/output/filtered_nuked_removed.pfm");

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }
    } else {
        printf("No, the file is not valid!\n");
    }

    return 0;
}
