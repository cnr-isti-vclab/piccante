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
    printf("Reading an HDR file...");

    std::string img_str;

    if(argc == 2) {
        img_str = argv[1];
    } else {
        img_str = "../data/input/bottles.hdr";
    }

    pic::Image img;
    img.Read(img_str);

    printf("Ok\n");

    printf("Is it valid? ");
    if(img.isValid()) {
        printf("OK\n");

        pic::Image img_black(1, 32, 32, 3);
        img_black.setZero();

        //add a hole in the image
        srand (time(NULL));
        img.copySubImage(&img_black, (rand() % img.width) - 32, (rand() % img.height) - 32);

        auto name = pic::getFileNameOnly(img_str);
        auto ext = pic::getExtension(img_str);

        printf("%s %s\n", name.c_str(), ext.c_str());

        img.Write("../data/output/" + name + "_pp_black_pixels." + ext);

        //recover black pixels with push-pull
        pic::PushPull pp;

        pic::Image *imgOut = pp.execute(&img, NULL, 0.0f);

        printf("Writing recovered result using Push-Pull... ");

        bool bWritten = imgOut->Write("../data/output/" + name + "_pp_reconstruction." + ext);

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }
    } else {
        printf("No, the file is not valid!\n");
    }

    return 0;
}
