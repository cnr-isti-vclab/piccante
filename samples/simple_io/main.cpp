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
    img.Read("../data/input/bottles.hdr");
    printf("Ok\n");

    printf("Is it valid? ");
    if(img.isValid()) {
        printf("OK\n");

        //Testing native writing formats
        pic::StringVec outputFormats;
        outputFormats.push_back("bmp");
        outputFormats.push_back("tga");
        outputFormats.push_back("ppm");
        outputFormats.push_back("pgm");
        outputFormats.push_back("pfm");
        outputFormats.push_back("hdr");

        for(unsigned int i=0; i<outputFormats.size(); i++) {
            std::string text = "Writing the file to disk as " + outputFormats[i] + " file... ";
            printf("%s", text.c_str());
            bool bWritten = img.Write("../data/output/bottles." + outputFormats[i]);

            if(bWritten) {
                printf("Ok\n");
            } else {
                printf("Writing had some issues!\n");
            }
        }
    } else {
        printf("No, the file is not valid!\n");
    }

    return 0;
}
