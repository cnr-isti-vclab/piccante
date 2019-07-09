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
    printf("Reading source and target images...");

    pic::Image img_source, img_target;

    img_source.Read("../data/input/histogram_matching/source.png", pic::LT_NOR_GAMMA);
    img_target.Read("../data/input/histogram_matching/target.png", pic::LT_NOR_GAMMA);

    printf("Ok\n");

    printf("Are these valid? ");
    if(img_source.isValid() && img_target.isValid()) {

        pic::Image *out = pic::HistogramMatching::execute(&img_source, &img_target, NULL);

        out->Write("../data/output/histogram_matching.png");
    } else {
        printf("No, the files are not valid!\n");
    }

    return 0;
}
