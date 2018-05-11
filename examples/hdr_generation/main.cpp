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
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    //exposure time in seconds: exposureTime[0] ==> img[0], etc.
    float exposure_time_vec[] = {2.0f, 1.0f / 2.0f, 1.0f / 8.0f, 1.0f / 30.0f, 1.0f / 125.0f, 1.0f / 1000.0f, 1.0f / 4000.0f};

    printf("Reading a stack of LDR images...");
    bool valid = true;
    pic::ImageVec stack_vec;

    for(int i = 0; i < 7; i++) {
        pic::Image *img = new pic::Image();
        std::string name = "../data/input/stack/stack_room_exp_" + pic::fromNumberToString(i) + ".jpg";
        printf("\n%s", name.c_str());
        ImageRead(name, img, pic::LT_NOR);

        valid = valid && img->isValid();

        img->exposure = pic::EstimateAverageLuminance(exposure_time_vec[i], 4.5f, 200.0f);

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
        pic::Image *imgOut = merger.ProcessP(stack_vec, NULL);

        printf("Ok\n");

        if(imgOut != NULL) {
            ImageWrite(imgOut, "../data/output/hdr_generation_image_log.hdr");
            pic::Image *imgToneMapped_reinhard = pic::ReinhardTMO(imgOut);
            ImageWrite(imgToneMapped_reinhard, "../data/output/image_debevec_crf_tone_mapped.png", pic::LT_NOR_GAMMA);
       //     delete imgToneMapped_reinhard;
        //    delete imgOut;
        }

        printf("Estimating the polynomial camera response function... ");

        bool ok = crf.MitsunagaNayar(stack_vec, -6, 256, true, 0.08f, true);

        if (ok) {
            printf("Ok.\n");

            printf("Assembling the different exposure images... ");
            fflush(stdout);
            pic::FilterAssembleHDR mergerPoly(&crf, pic::CW_DEB97, pic::HRD_LOG);
            imgOut = mergerPoly.ProcessP(stack_vec, NULL);

            printf("Ok\n");

            if(imgOut != NULL) {
                ImageWrite(imgOut, "../data/output/hdr_generation_image_poly.hdr");

                pic::Image *imgToneMapped_reinhard = pic::ReinhardTMO(imgOut);
                ImageWrite(imgToneMapped_reinhard, "../data/output/image_mitusunaga_crf_tone_mapped.png", pic::LT_NOR_GAMMA);

                delete imgToneMapped_reinhard;
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
