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
