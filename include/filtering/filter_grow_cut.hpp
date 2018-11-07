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

#ifndef PIC_FILTERING_FILTER_GROW_CUT_HPP
#define PIC_FILTERING_FILTER_GROW_CUT_HPP

#include "../filtering/filter.hpp"
#include "../util/array.hpp"

namespace pic {

/**
 * @brief The FilterGrowCut class
 */
class FilterGrowCut: public Filter
{
protected:

    int dx[8], dy[8];

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box)
    {                
        Image *state_cur  = src[0];
        Image *img        = src[1];
        Image *img_max    = src[2];

        Image *state_next  = dst;

        int channels = img->channels;

        for(int j = box->y0; j < box->y1; j++) {
            for(int i = box->x0; i < box->x1; i++) {
                float *s_cur = (*state_cur)(i, j);
                float *s_next = (*state_next)(i, j);
                float *col = (*img)(i, j);

                float C = (*img_max)(i, j)[0];

                s_next[0] = s_cur[0];
                s_next[1] = s_cur[1];

                for(int k = 0; k < 8; k++) {
                    int x = i + dx[k];
                    int y = j + dy[k];

                    float *s_cur_k = (*state_cur)(x, y);
                    float *col_k = (*img)(x, y);

                    float dist = Arrayf::distanceSq(col, col_k, channels);

                    float g_theta = 1.0f - (dist / C);
                    g_theta *= s_cur_k[1];

                    if(g_theta > s_cur[1]) {
                        s_next[0] = s_cur_k[0];
                        s_next[1] = g_theta;
                    }
                }
            }
        }
    }


public:

    /**
     * @brief FilterGrowCut
     */
    FilterGrowCut() : Filter()
    {
        int dx_t[8] = {-1, 0, 1, -1, 1, -1,  0,  1};
        int dy_t[8] = { 1, 1, 1,  0, 0, -1, -1, -1};

        memcpy(dx, dx_t, sizeof(int) * 8);
        memcpy(dy, dy_t, sizeof(int) * 8);
    }

};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_GROW_CUT_HPP */

