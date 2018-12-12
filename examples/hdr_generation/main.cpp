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


#include "piccante.hpp"

int main(int argc, char *argv[])
{
    printf("Reading a stack of LDR images...");
    bool valid = true;
    pic::ImageVec stack_vec;

    for(int i = 0; i < 7; i++) {
        pic::Image *img = new pic::Image();
        std::string name = "../data/input/stack/stack_room_exp_" + pic::fromNumberToString(i) + ".jpg";

        printf("\n%s", name.c_str());
        img->Read(name, pic::LT_NOR);

        valid = valid && img->isValid();

        stack_vec.push_back(img);
    }

    printf("\nOk\n");

    printf("Are these images valid? ");
    if(valid) {
        printf("Ok\n");

        pic::CameraResponseFunction crf;

        printf("Estimating the camera response function... ");
        crf.DebevecMalik(stack_vec);
        printf("Ok\n");

        printf("Assembling the different exposure images... ");
        pic::FilterAssembleHDR merger(&crf, pic::CW_DEB97, pic::HRD_LOG);
        pic::Image *imgOut = merger.Process(stack_vec, NULL);

        printf("Ok\n");

        pic::ReinhardTMO rtmo;

        if(imgOut != NULL) {
            imgOut->Write("../data/output/image_debevec_crf.hdr");

            pic::Image *imgTmo = rtmo.Process(imgOut, NULL);
            imgTmo->Write("../data/output/image_debevec_crf_tmo.png", pic::LT_NOR_GAMMA);
            delete imgTmo;
            delete imgOut;
        }

        printf("Estimating the polynomial camera response function... ");

        bool ok = crf.MitsunagaNayar(stack_vec, -6, 256, false, 0.08f, true);

        if (ok) {
            printf("Ok.\n");

            printf("Assembling the different exposure images... ");
            fflush(stdout);
            pic::FilterAssembleHDR mergerPoly(&crf, pic::CW_DEB97, pic::HRD_LOG);
            imgOut = mergerPoly.Process(stack_vec, NULL);

            printf("Ok\n");

            if(imgOut != NULL) {
                imgOut->Write("../data/output/image_mitusunaga_crf.hdr");

                pic::Image *imgTmo = rtmo.Process(imgOut, NULL);
                imgTmo->Write("../data/output/image_mitusunaga_crf_tmo.png", pic::LT_NOR_GAMMA);

                delete imgTmo;
                delete imgOut;
            }
        } else {
            printf("Camera Respose Function not computed.\n");
        }

    } else {
        printf("No, the files are not valid!\n");
    }

    return 0;
}
