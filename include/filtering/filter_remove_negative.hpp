/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

PICCANTE is free software; you can redistribute it and/or modify
under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 3.0 of
the License, or (at your option) any later version.

PICCANTE is distributed in the hope that it will be useful, but
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License
( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.

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

