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
            pic::Image *gc_seg = gc.execute(&img, seeds, NULL);

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
