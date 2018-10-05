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

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    printf("Reading an image...");

    pic::Image img, strokes;

    std::string img_str, strokes_str;

    if(argc == 3) {
        img_str = argv[1];
        strokes_str = argv[2];
    } else {
        img_str = "../data/input/yellow_flowers.png";
        strokes_str = "../data/input/yellow_flowers_segmentation_strokes.png";
    }

    img.Read(img_str);
    strokes.Read(strokes_str);

    printf("OK\n");

    printf("Are input images valid? ");
    if(img.isValid() && strokes.isValid()) {
        printf("OK\n");

        pic::GrowCut gc;

        pic::Image *seeds = pic::GrowCut::fromStrokeImageToSeeds(&strokes, NULL);

        if(seeds != NULL) {
            pic::Image *gc_seg = gc.execute(&img, seeds);

            std::string name = pic::getFileNameOnly(img_str);

            gc_seg->Write("../data/output/" + name + "_status.pfm");

            pic::Image *gc_mask = pic::GrowCut::getMaskAsImage(gc_seg, NULL);

            gc_mask->Write("../data/output/" + name + "_mask.png");
        }
    } else {
        printf("No, the file is not valid!\n");
    }

    return 0;
}
