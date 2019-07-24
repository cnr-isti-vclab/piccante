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
    std::string img0_str, img1_str;
    bool bCreate = false;

    if(argc == 3) {
        img0_str = argv[1];
        img1_str = argv[2];
    } else {
        img0_str = "../data/input/bottles.hdr";
        bCreate = true;
    }

    pic::Image img0, img1;
    img0.Read(img0_str, pic::LT_NOR);

    bool bLoaded = false;
    if(!bCreate) {
        //this an LDR image with values in [0,255]!
        bLoaded = img1.Read(img1_str, pic::LT_NONE);
    }

    printf("Is it valid? ");
    if(img0.isValid() && (bCreate || bLoaded)) {
        printf("OK\n");

        std::string name = pic::removeLocalPath(img0_str);
        name = pic::removeExtension(name);

        pic::Image *tmp = NULL;
        if(bCreate) {
            printf("Tone mapping the input image...");
            pic::DragoTMO dtmo;
            tmp = dtmo.Process(Single(&img0), tmp);
            printf("Ok\n");
            tmp->Write("../data/output/" + name + "_flt.png");

            tmp->applyFunction(pic::simple8bitWithGamma);
        } else {
            tmp = &img1;
        }        

        float Q, N, S;
        pic::TMQI tmqi;
        tmqi.execute(pic::Double(&img0, tmp), Q, N, S, NULL);
        printf("TMQI -- Q: %f N: %f S: %f\n", Q, N, S);
    } else {
        printf("No, the file is not valid!\n");
    }

    return 0;
}
