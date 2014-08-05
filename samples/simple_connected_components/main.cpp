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

//This means that we disable Eigen; some functionalities cannot be used.
//For example, estimating the camera response function
#define PIC_DISABLE_EIGEN
//This means that OpenGL acceleration layer is disabled
#define PIC_DISABLE_OPENGL

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    printf("Reading an LDR file...");

    pic::ImageRAW img;
    img.Read("../data/input/connected_test.png", pic::LT_NOR);

    printf("Ok\n");

    printf("Is it valid? ");
    if(img.isValid()) {
        printf("Ok\n");

        printf("Computing connected components...");

        std::vector<pic::LabelOutput> ret;
        pic::ImageRAW *comp = pic::ConnectedComponents(&img, ret, NULL, 0.05f);
        printf("Ok!\n");

        unsigned int areaMin = img.nPixels();
        for(unsigned int i=0; i<ret.size(); i++) {
            unsigned int areaTmp = ret[i].coords.size();
            if(areaMin > areaTmp) {
                areaMin = areaTmp;
            }
        }

        std::string out = "The size of the smallest circle is: " + pic::NumberToString(areaMin) + " pixels.\n";
        printf("%s", out.c_str());

        printf("Writing the connected component labeling results to a file on the disk...");
        bool bWritten = comp->Write("../data/output/connected_components.pfm");

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }
    } else {
        printf("No it is not a valid file!\n");
    }

    return 0;
}
