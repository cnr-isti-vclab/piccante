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
    pic::NelderMeadOptPositivePolynomial::test();

    std::string img_str;

    if(argc == 2) {
        img_str = argv[1];
    } else {
        img_str = "../data/input/tommaseo_statue.png";
    }

    std::vector< pic::Vec2i > out, out2;
    pic::Vec2i pS(227, 206);
    pic::Vec2i pE(221, 351);
    auto out_single_jni = pic::executeLiveWireSingleJNI(img_str, 227, 206, 221, 351);

    //how to use multiple LiveWire points
    pic::Vec2i pE1(221, 381);
    std::vector< pic::Vec2i > cp;
    cp.push_back(pS);
    cp.push_back(pE);
    cp.push_back(pE1);
    //pic::executeLiveWireMultipleJNI(img_str, cp, out2);

    pic::Image img(img_str, pic::LT_NOR_GAMMA);

    for(auto i = 0; i < out_single_jni.size(); i+=2) {

        float *tmp = img(out_single_jni[i], out_single_jni[i + 1]);

        tmp[0] = 0.0f;
        tmp[1] = 1.0f;
        tmp[2] = 0.0f;
    }

    img.Write("../data/output/s_livewire_single.png", pic::LT_NOR_GAMMA);


    pic::Polyline2i out2_s(out2);

    for(auto i = 0; i < out2.size(); i++) {
        float *tmp = img(out2[i][0], out2[i][1]);

        tmp[0] = 1.0f;
        tmp[1] = 0.0f;
        tmp[2] = 0.0f;
    }

    out2_s.simplify(32);
    for(auto i = 0; i < out2_s.points.size(); i++) {
        float *tmp = img(out2_s.points[i][0], out2_s.points[i][1]);

        tmp[0] = 0.0f;
        tmp[1] = 1.0f;
        tmp[2] = 0.0f;
    }

    img.Write("../data/output/s_livewire_multiple.png", pic::LT_NOR_GAMMA);

    return 0;
}
