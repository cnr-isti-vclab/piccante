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

#ifndef PIC_FILTERING_FILTER_TMQI_HPP
#define PIC_FILTERING_FILTER_TMQI_HPP

#include "../util/math.hpp"

#include "../filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterTMQI class
 */
class FilterTMQI: public Filter
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
        float C1 = 0.01f;
        float C2 = 10.0f;

        int width    = src[0]->width;
        int channels = src[0]->channels;

        for(int j = box->y0; j < box->y1; j++) {
            int c = j * width;

            for(int i = box->x0; i < box->x1; i++) {
                int ind = (c + i) *  channels;

                float sigma1 = src[0]->data[ind];
                float sigma1p = normalCDF(sigma1, u_hdr, sig_hdr);

                float sigma2 = src[1]->data[ind];
                float sigma2p = normalCDF(sigma2, u_ldr, sig_ldr);

                float sigma12 = src[2]->data[ind];

                float tmp =  (((2*sigma1p*sigma2p)+C1)/((sigma1p*sigma1p)+(sigma2p*sigma2p)+C1))*((sigma12+C2)/(sigma1*sigma2 + C2));
                dst->data[ind] = tmp;
            }
        }
    }

    float u_hdr, sig_hdr, u_ldr, sig_ldr;

public:

    /**
     * @brief FilterTMQI
     * @param type
     */
    FilterTMQI() : Filter()
    {
        minInputImages = 3;
    }

    /**
     * @brief MannosCSF
     * @param sf
     * @return
     */
    static float MannosCSF(float sf)
    {
        return 100.0f * 2.6f *
                (0.0192f + 0.114f * sf) *
                expf(-powf(0.114f * sf, 1.1f));
    }

    /**
     * @brief update
     */
    void update(float sf)
    {
        float CSF = MannosCSF(sf);
        u_hdr = 128.0f / (1.4f * CSF);
        sig_hdr = u_hdr / 3.0f;

        u_ldr = u_hdr;
        sig_ldr = u_ldr / 3.0f;
    }

};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_TMQI_HPP */

