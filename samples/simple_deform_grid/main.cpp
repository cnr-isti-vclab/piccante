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

//This means we do not use QT for I/O
#define PIC_DISABLE_QT

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    printf("Reading an HDR file...");

    pic::Image img;
    img.Read("../data/input/grid.bmp");
    printf("Ok\n");

    printf("Is it valid? ");
    if(img.isValid()) {
        printf("OK\n");

        pic::Image *grid_move = pic::FilterDeformGrid::getUniformGrid(16, 16);
        (*grid_move)(4,4)[0] += 1 / 32.0f;
        (*grid_move)(4,4)[1] += 1 / 32.0f;

        pic::FilterDeformGrid flt_dg(grid_move);

        pic::Image *out = flt_dg.ProcessP(Single(&img), NULL);

        out->Write("../data/output/grid_deform.bmp");
    } else {
        printf("No, the file is not valid!\n");
    }

    return 0;
}
