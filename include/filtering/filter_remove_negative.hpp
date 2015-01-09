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

#ifndef PIC_FILTERING_FILTER_REMOVE_NEGATIVE_HPP
#define PIC_FILTERING_FILTER_REMOVE_NEGATIVE_HPP

#include "filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterRemoveNegative class
 */
class FilterRemoveNegative: public Filter
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
        int channels = dst->channels;

        for(int j = box->y0; j < box->y1; j++) {
            for(int i = box->x0; i < box->x1; i++) {

                float *tmp_data = (*src[0])(i, j);
                float *tmp_dst  = (*dst   )(i, j);
            
                for(int ch = 0; ch < channels; ch++) {
                    tmp_dst[ch] = tmp_data[ch] > 0.0f ? tmp_data[ch] : 0.0f;
                }
            }
        }
    }
     

public:
    /**
     * @brief FilterRemoveNegative
     */
    FilterRemoveNegative()
    {
        this->threshold_nuked = threshold_nuked;
    }

    /**
     * @brief Execute
     * @param imgIn
     * @param imgOut
     * @param threshold_nuked
     * @return
     */
    static Image* Execute(Image *imgIn, Image *imgOut)
    {
        FilterRemoveNegative filter;
        imgOut = filter.ProcessP(Single(imgIn), imgOut);
        return imgOut;
    }

    /**
     * @brief Execute
     * @param nameFileIn
     * @param nameFileOut
     * @return
     */
    static Image* Execute(std::string nameFileIn, std::string nameFileOut)
    {
        Image imgIn(nameFileIn);
        Image *imgOut = Execute(&imgIn, NULL);
        imgOut->Write(nameFileOut);

        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_REMOVE_NEGATIVE_HPP */

