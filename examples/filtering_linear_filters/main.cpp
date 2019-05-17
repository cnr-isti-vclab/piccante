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

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    std::string name_str = "../data/input/bottles.hdr";
    if(argc == 2) {
        name_str = argv[1];
    }

    printf("Reading an HDR file...");

    pic::Image img;
    img.Read(name_str);

    printf("Ok\n");

    printf("Is it valid? ");
    if(img.isValid()) {
        printf("Ok\n");

        printf("Filtering the image with a Gaussian filter with sigma = 4.0...");
        pic::Image *output = pic::FilterGaussian2D::execute(&img, NULL, 4.0f);

        printf("Ok!\n");

        auto name = pic::getFileNameOnly(name_str);

        printf("Writing the file to disk...");
        bool bWritten = output->Write("../data/output/" + name + "_filtered_gaussian_4_0.hdr");

        printf("Filtering the image with a LoG filter with sigma = 2.0...");
        pic::Image *L = pic::FilterLuminance::execute(&img, NULL, pic::LT_CIE_LUMINANCE);
        pic::Image *L_log = pic::FilterLoG2D::execute(L, NULL, 4.0f);

        printf("Ok!\n");

        printf("Writing the file to disk...");
        bWritten = L_log->Write("../data/output/" + name + "_filtered_log_2_0.hdr");

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }

        pic::Image *edges = pic::FilterZeroCrossing::execute(L_log, NULL);
        bWritten = edges->Write("../data/output/" + name + "filtered_log_2_0_edges.hdr");

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }

        pic::Image *sampling_map = pic::FilterSamplingMap::execute(&img, NULL, 8.0f);
        bWritten = sampling_map->Write("../data/output/" + name + "_sampling_map.hdr");

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
