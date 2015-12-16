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

    pic::Image img;
    img.Read("../data/input/grid.png");
    printf("Ok\n");

    printf("Is it valid? ");
    if(img.isValid()) {
        printf("OK\n");

        pic::Image *grid_move = pic::FilterDeformGrid::getUniformGrid(17, 17);
        float *grid = (*grid_move)(4, 4);
        grid[0] += 1 / 32.0f;
        grid[1] += 1 / 32.0f;

        pic::FilterDeformGrid flt_dg(grid_move);

        pic::Image *out = flt_dg.ProcessP(Single(&img), NULL);

        bool bWrite = out->Write("../data/output/deform_grid.png");

        if(bWrite) {
            printf("The output was not written\n");
        } else {
            printf("The output was written sucessfully!\n");
        }
    } else {
        printf("No, the file is not valid!\n");
    }

    return 0;
}
