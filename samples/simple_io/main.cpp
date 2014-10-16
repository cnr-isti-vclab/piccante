/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

PICCANTE is free software; you can redistribute it and/or modify
under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 3.0 of
the License, or (at your option) any later version.

PICCANTE is distributed in the hope that it will be useful, but
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License
( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.

*/

#include <QCoreApplication>

//This means that OpenGL acceleration layer is disabled
#define PIC_DISABLE_OPENGL
//This means we do not use QT for I/O
#define PIC_DISABLE_QT

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

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
            bool bWritten = img.Write("../data/output/bottles."+outputFormats[i]);

            if(bWritten) {
                printf("Ok\n");
            } else {
                printf("Writing had some issues!\n");
            }
        }
    } else {
        printf("No it is not a valid file!\n");
    }

    return 0;
}
