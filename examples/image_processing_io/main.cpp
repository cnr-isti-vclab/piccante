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

#define PIC_DISABLE_TINY_EXR
#define PIC_ENABLE_INLINING

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    std::string img_str = "";

    if(argc == 2) {
        img_str = argv[1];
    } else {
        img_str = "../data/input/bottles.hdr";
    }

    printf("Reading a .hdr file...");

    pic::Image img;
    img.Read(img_str);
    printf("Ok\n");

    printf("Is it valid? ");
    if(img.isValid()) {
        printf("OK\n");

        //test native writing formats
        pic::StringVec outputFormats;
        outputFormats.push_back("bmp");
        outputFormats.push_back("tga");
        outputFormats.push_back("ppm");
        outputFormats.push_back("pgm");
        outputFormats.push_back("pfm");
        outputFormats.push_back("hdr");
        outputFormats.push_back("exr");

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
