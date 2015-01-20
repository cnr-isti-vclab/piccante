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

    pic::Image img;
    img.Read("../data/input/connected_test.png", pic::LT_NOR);

    printf("Ok\n");

    printf("Is it valid? ");
    if(img.isValid()) {
        printf("Ok\n");

        printf("Computing connected components...");

        std::vector<pic::LabelOutput> ret;
        pic::Image *comp = pic::ConnectedComponents(&img, ret, NULL, 0.05f);
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
        printf("No, the file is not valid!\n");
    }

    return 0;
}
