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

#ifndef PIC_FILTERING_FILTER_SSIM_HPP
#define PIC_FILTERING_FILTER_SSIM_HPP

#include "../filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterSSIM class
 */
class FilterSSIM: public Filter
{
protected:

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box)
    {
        int width    = src[0]->width;
        int channels = src[0]->channels;

        for(int j = box->y0; j < box->y1; j++) {
            int c = j * width;

            for(int i = box->x0; i < box->x1; i++) {
                int ind = (c + i) *  channels;

                float mu1 = src[0]->data[ind];
                float mu1_sq = mu1 * mu1;

                float mu2 = src[1]->data[ind];
                float mu2_sq = mu2 * mu2;

                float sigma1_sq = src[2]->data[ind] - mu1_sq;
                float sigma2_sq = src[3]->data[ind] - mu2_sq;
                float mu1_mu2 = mu1 * mu2;
                float sigma1_sigma2 = src[4]->data[ind] - mu1_mu2;

                //numerator
               float tmp1 = (mu1_mu2 * 2.0f + C0) *
                            (sigma1_sigma2 * 2.0f + C1);

               //denominator
               float tmp2 = (mu1_sq + mu2_sq + C0 ) *
                            (sigma1_sq + sigma2_sq + C1);

               dst->data[ind] = tmp1 / tmp2;
            }
        }
    }

    float C0, C1;

public:

    /**
     * @brief FilterSSIM
     * @param type
     */
    FilterSSIM() : Filter()
    {
        minInputImages = 5;
    }

    /**
     * @brief FilterSSIM
     * @param type
     */
    FilterSSIM(float C0, float C1) : Filter()
    {
        minInputImages = 5;
        update(C0, C1);
    }

    /**
     * @brief update
     * @param C0
     * @param C1
     */
    void update(float C0, float C1)
    {
        this->C0 = C0;
        this->C1 = C1;
    }

};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_SSIM_HPP */

