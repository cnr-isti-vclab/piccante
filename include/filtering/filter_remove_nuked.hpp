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

#ifndef PIC_FILTERING_FILTER_REMOVE_NUKED_HPP
#define PIC_FILTERING_FILTER_REMOVE_NUKED_HPP

#include "../filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterRemoveNuked class
 */
class FilterRemoveNuked: public Filter
{
protected:
    float threshold_nuked;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box)
    {
        float maxVal;
        float values[9];

        int channels = dst->channels;

        for(int j = box->y0; j < box->y1; j++) {
            for(int i = box->x0; i < box->x1; i++) {

                float *tmp_data = (*src[0])(i, j);
                float *tmp_dst  = (*dst   )(i, j);
            
                for(int ch = 0; ch < channels; ch++) {

                    maxVal = -FLT_MAX;
                    int c2 = 0;
                    int nuked = 0;
                    float val = tmp_data[ch];

                    for(int k = -1; k <= 1; k++) {
                        for(int l = -1; l <= 1; l++) {

                            float *tmp_val = (*src[0])(i + l, j + k);
                            values[c2] = tmp_val[ch];

                            float t_new = threshold_nuked * tmp_val[ch];
                            if(fabsf(tmp_val[ch] - tmp_data[ch]) > t_new) {
                                nuked++;
                            }

                            c2++;
                        }
                    }

                    if(nuked > 5) {//are nuked pixels the majority?
                        std::sort(values, values + 9);
                        tmp_dst[ch] = values[5];
                    } else {
                        tmp_dst[ch] = val;
                    }
                }
            }
        }
    }
     

public:
    /**
     * @brief FilterRemoveNuked
     * @param threshold_nuked
     */
    FilterRemoveNuked(float threshold_nuked = 1e4f)
    {
        this->threshold_nuked = threshold_nuked;
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param threshold_nuked
     * @return
     */
    static Image* execute(Image *imgIn, Image *imgOut, float threshold_nuked = 1e4)
    {
        FilterRemoveNuked filter(threshold_nuked);
        imgOut = filter.Process(Single(imgIn), imgOut);
        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_REMOVE_NUKED_HPP */

