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

#include "../common_code/image_qimage_interop.hpp"

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    std::string img_str;
    if(argc == 2) {
        img_str = argv[1];
    } else {
        img_str = "../data/input/grid.png";
    }

    printf("Reading an image...");
    pic::Image img;
    ImageRead(img_str, &img, pic::LT_NOR);

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

        bool bWrite = ImageWrite(out, "../data/output/grid_output_deformation.png", pic::LT_NOR);

        if(bWrite) {
            printf("The output was written sucessfully!\n");
        } else {
            printf("The output was not written\n");
        }
    } else {
        printf("No, the file is not valid!\n");
    }

    return 0;
}
