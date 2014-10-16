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

#ifndef PIC_FILTERING_FILTER_REMOVE_NUKED_HPP
#define PIC_FILTERING_FILTER_REMOVE_NUKED_HPP

#include "filtering/filter.hpp"

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

                            if(fabsf(tmp_val[ch] - tmp_data[ch]) > threshold_nuked) {
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
     * @brief Execute
     * @param imgIn
     * @param imgOut
     * @param threshold_nuked
     * @return
     */
    static Image* Execute(Image *imgIn, Image *imgOut, float threshold_nuked = 1e4)
    {
        FilterRemoveNuked filter(threshold_nuked);
        imgOut = filter.ProcessP(Single(imgIn), imgOut);
        return imgOut;
    }

    /**
     * @brief Execute
     * @param nameFileIn
     * @param nameFileOut
     * @param threshold_nuked
     * @return
     */
    static Image* Execute(std::string nameFileIn, std::string nameFileOut, float threshold_nuked = 1e4)
    {
        Image imgIn(nameFileIn);
        Image *imgOut = Execute(&imgIn, NULL, threshold_nuked);
        imgOut->Write(nameFileOut);

        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_REMOVE_NUKED_HPP */

