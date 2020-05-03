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
    printf("Adding file names to the merger...");
    pic::HDRMerger merger;

    for(int i = 0; i < 7; i++) {
        std::string name = "../data/input/stack/stack_room_exp_" + pic::fromNumberToString(i) + ".jpg";
        merger.addFile(name);
    }

    printf("\nOk\n");

    printf("Merging LDR images into an HDR image...");
    pic::Image *imgOut = merger.execute(false, NULL);
    printf("\nOk\n");

    if(imgOut != NULL) {
        if(imgOut->isValid()) {
            imgOut->Write("../data/output/image_debevec_crf.hdr");
            pic::Image *imgTmo = pic::ReinhardTMO::executeGlobal1(imgOut, NULL);
            imgTmo->Write("../data/output/image_debevec_crf_tmo.png", pic::LT_NOR_GAMMA);
            delete imgTmo;
            delete imgOut;
        }
    }

    return 0;
}
