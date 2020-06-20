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

    std::string img_source_str, img_target_str, img_out_str;

    if(argc == 4) {
        img_source_str = argv[1];
        img_target_str = argv[2];
        img_out_str = argv[3];
    } else {
        img_source_str = "../data/input/histogram_matching/source.png";
        img_target_str = "../data/input/histogram_matching/target.png";
        img_out_str = "../data/output/histogram_matching.png";
    }

    img_source.Read(img_source_str, pic::LT_NOR_GAMMA);
    img_target.Read(img_target_str, pic::LT_NOR_GAMMA);

    printf("Ok\n");

    printf("Are these valid? ");
    if(img_source.isValid() && img_target.isValid()) {

        pic::Image *out = pic::HistogramMatching::execute(&img_source, &img_target, NULL);
        out->Write(img_out_str);

        auto name = pic::removeExtension(img_out_str);

        pic::Image *out_eq_1 = pic::HistogramMatching::executeEqualization(&img_source);
        out_eq_1->Write(name + "_source_equalization.png");

        pic::Image *out_eq_2 = pic::HistogramMatching::executeEqualization(&img_target);
        out_eq_2->Write(name + "_target_equalization.png");

        pic::Image *out_eq_1_local = pic::FilerCLAHE::execute(&img_source, NULL, 128);
        out_eq_1_local->Write(name + "_target_clahe.png");

    } else {
        printf("No, the files are not valid!\n");
    }

    return 0;
}
