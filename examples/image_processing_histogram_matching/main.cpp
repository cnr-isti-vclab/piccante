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

#include "../common_code/image_qimage_interop.hpp"

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    printf("Reading source and target images...");

    pic::Image img_source, img_target;

    ImageRead("../data/input/histogram_matching/source.png", &img_source);
    ImageRead("../data/input/histogram_matching/target.png", &img_target);

    printf("Ok\n");

    printf("Are these valid? ");
    if(img_source.isValid() && img_target.isValid()) {

        pic::Image *out = pic::matchHistograms(&img_source, &img_target);

        ImageWrite(out, "../data/output/histogram_matching.png");
    } else {
        printf("No, the files are not valid!\n");
    }

    return 0;
}
