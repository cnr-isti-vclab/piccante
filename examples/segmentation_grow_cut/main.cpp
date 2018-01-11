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

#include <QCoreApplication>

//This means that OpenGL acceleration layer is disabled
#define PIC_DISABLE_OPENGL

#include "../common_code/image_qimage_interop.hpp"

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    printf("Reading an image...");

    pic::Image img, strokes;

    ImageRead("../data/input/yellow_flowers.png", &img);
    ImageRead("../data/input/yellow_flowers_segmentation_strokes.png", &strokes);

    printf("OK\n");

    printf("Are input images valid? ");
    if(img.isValid() && strokes.isValid()) {
        printf("OK\n");

        pic::Image *gc = pic::computeGrowCut(&img, &strokes);

        gc->Write("../data/output/s_grow_cut.pfm");


    } else {
        printf("No, the file is not valid!\n");
    }

    return 0;
}
