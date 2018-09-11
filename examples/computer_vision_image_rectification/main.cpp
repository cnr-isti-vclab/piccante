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

#define EIGEN_DONT_VECTORIZE

#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT

#include "piccante.hpp"




int main(int argc, char *argv[])
{   
    printf("Reading an LDR images...");

    Eigen::Matrix34d P0, P1;
    pic::Image img0, img1;
    std::string name0, name1;

    if(argc != 5) {
        name0 = "../data/input/triangulation/campo_s_stefano_r.jpg";
        name1 = "../data/input/triangulation/campo_s_stefano_l.jpg";
        P0 = pic::readMatrix34dFromFile("../data/input/triangulation/campo_s_stefano_r_cam.txt");
        P1 = pic::readMatrix34dFromFile("../data/input/triangulation/campo_s_stefano_l_cam.txt");
    } else {
        name0 = argv[1];
        name1 = argv[2];
        P0 = pic::readMatrix34dFromFile(argv[3]);
        P1 = pic::readMatrix34dFromFile(argv[4]);
    }

    img0.Read(name0, pic::LT_NOR);
    img1.Read(name1, pic::LT_NOR);

    printf("Ok\n");
    
    printf("Are images both valid? ");
    if(img0.isValid() && img1.isValid()) {
        printf("OK\n");
        
        pic::ImageVec *out = pic::computeImageRectification(&img0, &img1, P0, P1, NULL, true);

        name0 = pic::getFileNameOnly(name0);
        name1 = pic::getFileNameOnly(name1);

        out->at(0)->Write("../data/output/" + name0 + "_rectified.png", pic::LT_NOR);
        out->at(1)->Write("../data/output/" + name1 + "_rectified.png", pic::LT_NOR);
    } else {
        printf("No there is at least an invalid file!\n");
    }
    
    return 0;
}
