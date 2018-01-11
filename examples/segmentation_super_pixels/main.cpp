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

//This means that OpenGL acceleration layer is disabled
#define PIC_DISABLE_OPENGL

#define PIC_DEBUG

#include "../common_code/image_qimage_interop.hpp"

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    pic::Image img;
    ImageRead("../data/input/tommaseo_statue.png", &img);

    if(img.isValid()) {
        pic::Slic slic(&img, 128);
        pic::Image *output = slic.getMeanImage(NULL);

        ImageWrite(output, "../data/output/s_slic.png");

    } else {
        printf("No, the file is not valid!\n");
    }

    return 0;
}
