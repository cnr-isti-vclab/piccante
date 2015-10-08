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

#ifndef PIC_ALGORITHMS_GROW_CUT_HPP
#define PIC_ALGORITHMS_GROW_CUT_HPP

#include "image.hpp"
#include "filtering/filter_laplacian.hpp"
#include "filtering/filter_max.hpp"

namespace pic {

void InitState(Image *state, Image *seeds)
{
    for(int i = 0; i < state->nPixels(); i++) {
        int j = i * 2;
        state->data[j] = seeds->data[i];

        if(seeds->data[i] > 0.0f) {
            state->data[j + 1] = 1.0f;
        } else {
            state->data[j + 1] = 0.0f;
        }
    }
}

Image *GrowCut(Image *img, Image *seeds, Image *mask)
{
    Image *state_cur = new Image(img->width, img->height, 2);
    InitState(state_cur, seeds);

    Image *img_max = FilterMax::Execute(img, NULL, 5);

    Image *state_next = state_cur->AllocateSimilarOne();

/*    float dx[] = {-1, 1,  0, 0};
    float dy[] = { 0, 0, -1, 1};*/

    float dx[] = {-1, 0, 1, -1, 1, -1,  0,  1};
    float dy[] = { 1, 1, 1,  0, 0, -1, -1, -1};

    int iterations = int(sqrtf(img->widthf * img->widthf + img->heightf * img->heightf));
    int channels = img->channels;

    for(int p = 0; p < iterations; p++) {
        bool bNotChanged = true;

        for(int i = 0; i < img->height; i++) {
            for(int j = 0; j < img->width; j++) {

                float *s_cur = (*state_cur)(j, i);
                float *s_next = (*state_next)(j, i);
                float *col = (*img)(j, i);

                float *col_max = (*img_max)(j, i);
                float C = col_max[0] * col_max[0];
                for(int c = 1; c < channels; c++) {
                    C += col_max[c] * col_max[c];
                }

                s_next[0] = s_cur[0];
                s_next[1] = s_cur[1];

                for(int k = 0; k < 8; k++) {
                    int x = j + dx[k];
                    int y = i + dy[k];

                    float *s_cur_k = (*state_cur)(x, y);
                    float *col_k = (*img)(x, y);

                    float dist = 0.0f;
                    for(int c = 0; c < channels; c++) {
                        float tmp = col[c] - col_k[c];
                        dist += tmp * tmp;
                    }

                    float g_theta = 1.0f - (dist / C);

                    g_theta *= s_cur_k[1];

                    if(g_theta > s_cur[1]) {
                        s_next[0] = s_cur_k[0];
                        s_next[1] = g_theta;
                        bNotChanged = false;
                    }
                }
            }

        }

        if(bNotChanged) {
            delete img_max;
            delete state_next;
            return state_cur;
        }

        Image *tmp = state_cur;
        state_cur = state_next;
        state_next = tmp;
    }

    delete img_max;
    delete state_next;

    return state_cur;
}

} // end namespace pic

#endif /* PIC_ALGORITHMS_GROW_CUT_HPP */

