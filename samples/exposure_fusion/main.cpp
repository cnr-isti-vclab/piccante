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

        printf("Computing HDR Image histogram...");

        pic::ImageVec input = Single(&img);
        pic::FilterLuminance fLum;
        pic::Image *lum = fLum.ProcessP(input, NULL);

        pic::Histogram hst;
        hst.Calculate(lum, pic::VS_LOG_2, 256, 0);
        printf("Ok\n");

        printf("Extracting exposures from the HDR image...");
        pic::FilterSimpleTMO fltTMO(2.2f, 0.0f);
        std::vector< float > exposures = hst.ExposureCovering(8, 0.5f);
        pic::ImageVec stack;
        for(unsigned int i = 0; i < exposures.size(); i++) {
            fltTMO.Update(2.2f, exposures[i]);
            pic::Image *tmp = fltTMO.ProcessP(input, NULL);
            tmp->clamp(0.0f, 1.0f);
            stack.push_back(tmp);

            //writing the extraced exposure image
            tmp->Write("../data/output/exposure_" + pic::NumberToString(i) + ".png", pic::LT_NOR);
        }

        printf("Ok\n");

        printf("Tone mapping using Exposure Fusion...");
        pic::Image *imgToneMapped = pic::ExposureFusion(stack, NULL, 0.2f, 1.0f, 0.2f);
        printf("Ok\n");

        printf("Writing the tone mapped image to disk...\n");
        bool bWritten = imgToneMapped->Write("../data/output/exposure_fusion_tmo.png", pic::LT_NOR);

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
