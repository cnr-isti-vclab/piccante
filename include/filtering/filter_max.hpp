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

#ifndef PIC_FILTERING_FILTER_MAX_HPP
#define PIC_FILTERING_FILTER_MAX_HPP

#include "filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterMax class
 */
class FilterMax: public Filter
{
protected:
    int halfSize;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box)
    {
        int width = dst->width;
        int channels = dst->channels;

        float *data = src[0]->data;
        
        for(int j = box->y0; j < box->y1; j++) {
            int ind = j * width;

            for(int i = box->x0; i < box->x1; i++) {
                //Central weight
                int c = (ind + i) * channels;

                for(int ch = 0; ch < channels; ch++) {
                    float maxVal = -FLT_MAX;

                    for(int k = -halfSize; k <= halfSize; k++) {
                        int ci = CLAMP((i + k), dst->width);

                        for(int l = -halfSize; l <= halfSize; l++) {
                            int cj = CLAMP((j + l), dst->height);
                            float tmp = data[(cj * width + ci) * channels + ch];
                            maxVal = maxVal > tmp ? maxVal : tmp;
                        }
                    }

                    dst->data[c + ch] = maxVal;
                }
            }
        }
    }

public:
    /**
     * @brief FilterMax
     * @param size
     */
    FilterMax(int size)
    {
        this->halfSize = checkHalfSize(size);
    }

    /**
     * @brief Execute
     * @param imgIn
     * @param imgOut
     * @param size
     * @return
     */
    static Image *Execute(Image *imgIn, Image *imgOut, int size)
    {
        FilterMax filter(size);
        return filter.ProcessP(Single(imgIn), imgOut);
    }

    /**
     * @brief Execute
     * @param nameIn
     * @param nameOut
     * @param size
     * @return
     */
    static Image *Execute(std::string nameIn, std::string nameOut, int size)
    {
        Image imgIn(nameIn);
        Image *imgOut = Execute(&imgIn, NULL, size);
        imgOut->Write(nameOut);
        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_MAX_HPP */

