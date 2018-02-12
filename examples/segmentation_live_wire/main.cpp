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
        img_str = "../data/input/tommaseo_statue.png";
    }

    printf("Reading an image...");
    pic::Image img;
    ImageRead(img_str, &img);

    if(img.isValid()) {
        printf(" Ok!\n");
        pic::LiveWire *lw = new pic::LiveWire(&img);

        std::vector< pic::Vec<2, int> > out;
        pic::Vec<2, int> pS(227, 206);
        pic::Vec<2, int> pE(221, 351);
        lw->execute(pS, pE, out);

        for(unsigned int i = 0; i < out.size(); i++) {
            float *tmp = img(out[i][0], out[i][1]);

            tmp[0] = 1.0f;
            tmp[1] = 0.0f;
            tmp[2] = 0.0f;
        }

        ImageWrite(&img, "../data/output/s_livewire.png");
    } else {
        printf("No, the file is not valid!\n");
    }

    return 0;
}
