/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle










This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

*/

#ifndef PIC_UTIL_RASTERIZER_HPP
#define PIC_UTIL_RASTERIZER_HPP

#include <algorithm>
#include <cstdlib>

#include "image.hpp"
#include "util/vec.hpp"
#include "util/math.hpp"

namespace pic {

void DrawLine(Image *img, Vec<2, int> v0, Vec<2, int> v1, float *color)
{
    if(img == NULL || color == NULL) {
        return;
    }

    CLAMP(v0[0], img->width);
    CLAMP(v1[0], img->width);
    CLAMP(v0[1], img->height);
    CLAMP(v1[1], img->height);

    float *data = img->data;

    if(v0[0] > v1[0]) {
        std::swap(v0, v1);
    }

    int dx = v1[0] - v0[0];
    int dy = v1[1] - v0[1];

    //Vertical line
    if(dx == 0){
        if(v0[1] > v1[1]) {
            std::swap(v0, v1);
        }

        for(int y = v0[1]; y < v1[1]; y++) {
            int ind = (v0[0] + y * img->width) * img->channels;

            for(int k = 0; k < img->channels; k++) {
                data[ind + k] = color[k];
            }
        }
        return;
    }

    //Horizontal line
    if(dy == 0) {
        int ind_y = v0[1] * img->width;

        for(int x = v0[0]; x < v1[0]; x++) {
            int ind = (x + ind_y) * img->channels;

            for(int k = 0; k < img->channels; k++) {
                data[ind + k] = color[k];
            }
        }
    }

    //General case
    if(std::abs(dy) < std::abs(dx)) {
        //m < 1
        int e = 0;
        int s;
        if(dy < 0) {
            s  = -1;
            dy = -dy;
        } else {
            s = 1;
        }

        int y = v0[1];
        for(int x = v0[0]; x <= v1[0]; x++) {
            int ind = (x + y * img->width) * img->channels;

            for(int k = 0; k < img->channels; k++) {
                data[ind + k] = color[k];
            }

            e += dy;
            if((e << 1) >= dx) {
              y += s;
              e -= dx;
            }
        }
    } else {
        //m > 1
        if(v0[1] > v1[1]){
            std::swap(v0, v1);
        }

        dx = v1[0] - v0[0];
        dy = v1[1] - v0[1];

        int e = 0;
        int s;
        if(dx < 0){
            s  = -1;
            dx = -dx;
        } else {
            s = 1;
        }

        int x = v0[0];

        for(int y = v0[1]; y <= v1[1]; y++) {

            int ind = (x + y * img->width) * img->channels;

            for(int k = 0; k < img->channels; k++) {
                data[ind + k] = color[k];
            }

            e += dx;
            if((e << 1) >= dy) {
              x += s;
              e -= dy;
            }
        }
    }
}

}

#endif //
