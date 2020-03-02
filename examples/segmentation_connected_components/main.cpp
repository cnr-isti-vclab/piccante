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

//This means that we disable Eigen; some functionalities cannot be used.
//For example, estimating the camera response function
#define PIC_DISABLE_EIGEN
//This means that OpenGL acceleration layer is disabled
#define PIC_DISABLE_OPENGL

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    std::string img_name_str;

    if(argc > 1) {
        img_name_str = argv[1];
    } else {
        img_name_str = "../data/input/connected_test.png";
    }

    printf("Reading an LDR file...");

    pic::Image img;
    img.Read(img_name_str, pic::LT_NOR);

    printf("Ok\n");

    printf("Is it valid? ");
    if(img.isValid()) {
        printf("Ok\n");

        printf("Computing connected components...");

        std::vector<pic::LabelOutput> ret;
        pic::ConnectedComponents cc;

        float color[] = {0.0f, 0.0f, 0.0f};
        auto mask = img.convertToMask(color, 0.0f, true, NULL);

        pic::Image tmp;
        tmp.convertFromMask(mask, img.width, img.height);

        auto img_cc = cc.execute(mask, img.width, img.height, NULL, ret);

        printf("Ok!\n");

        unsigned int areaMin = img.nPixels();
        for(unsigned int i = 0; i < ret.size(); i++) {
            unsigned int areaTmp = ret[i].coords.size();
            if(areaMin > areaTmp) {
                areaMin = areaTmp;
            }
        }

        std::string out = "The size of the smallest circle is: " + pic::fromNumberToString(areaMin) + " pixels.\n";
        printf("%s", out.c_str());

        printf("Writing the connected component labeling results to a file on the disk...");

        pic::Image *comp = pic::ConnectedComponents::convertFromIntegerToImage(img_cc, NULL, img.width, img.height);
        bool bWritten = comp->Write("../data/output/connected_components.pfm");

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
