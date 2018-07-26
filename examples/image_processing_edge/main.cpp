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

    if(argc == 2) {
        img_str = argv[1];
    } else {
        img_str = "../data/input/features/checker_board_photo_2.png";
    }

    printf("Reading an LDR file...");

    pic::Image img(img_str);

    printf("Ok\n");

    printf("Is it valid? ");
    if(img.isValid()) {
        printf("OK\n");

        bool bWritten;
        std::string name = pic::getFileNameOnly(img_str);

        pic::CannyEdgeDetector ced;

        pic::Image *img_edges = ced.execute(&img, NULL);

        bWritten = img_edges->Write("../data/output/" + name + "_edges.png");

        bool *mask = img_edges->convertToMask(NULL, 0.5f, true, NULL);

        bool *mask_thin = pic::Mask::thinning(NULL, mask, img.width, img.height);

        img_edges->convertFromMask(mask_thin, img.width, img.height);

        bWritten = img_edges->Write("../data/output/" + name + "_edges_thin.png");

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
