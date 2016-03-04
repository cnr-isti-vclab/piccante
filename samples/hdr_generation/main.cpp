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

//This means that OpenGL acceleration layer is disabled
#define PIC_DISABLE_OPENGL

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    printf("Reading a stack of LDR images...");
    //reading images and storing them with normalized values in [0,1]
    pic::Image img[7];
    img[0].Read("../data/input/stack/stack_room_exp_0.jpg", pic::LT_NOR);
    img[1].Read("../data/input/stack/stack_room_exp_1.jpg", pic::LT_NOR);
    img[2].Read("../data/input/stack/stack_room_exp_2.jpg", pic::LT_NOR);
    img[3].Read("../data/input/stack/stack_room_exp_3.jpg", pic::LT_NOR);
    img[4].Read("../data/input/stack/stack_room_exp_4.jpg", pic::LT_NOR);
    img[5].Read("../data/input/stack/stack_room_exp_5.jpg", pic::LT_NOR);
    img[6].Read("../data/input/stack/stack_room_exp_6.jpg", pic::LT_NOR);

    //exposure time in seconds: exposureTime[0] ==> img[0], etc.
    float exposure_time_vec[] = {1.5802, 0.3951, 0.0988, 0.0263, 0.0063, 0.0008, 0.0002};

    bool valid = true;
    pic::ImageVec stack_vec;
    for(int i=0; i<7; i++) {
        valid = valid && img[i].isValid();

        img[i].exposure = exposure_time_vec[i];
        stack_vec.push_back(&img[i]);
    }

    printf("Ok\n");

    printf("Are these images valid? ");
    if(valid) {
        printf("Ok\n");

        //Estimating the camera response function
        printf("Estimating the camera response function... ");
        pic::CameraResponseFunction crf;
        crf.DebevecMalik(stack_vec, exposure_time_vec);
        printf("Ok.\n");

        printf("Assembling the different exposure images... ");
        pic::FilterAssembleHDR merger(pic::CW_DEB97, pic::HRD_LOG, pic::IL_LUT_8_BIT, &crf.icrf);
        pic::Image *imgOut = merger.ProcessP(stack_vec, NULL);

        printf("Ok\n");

        if(imgOut != NULL) {
            imgOut->Write("../data/output/hdr_generation_image.hdr");
        }

    } else {
        printf("No, the files are not valid!\n");
    }

    return 0;
}
