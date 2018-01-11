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

#ifndef PIC_FILTERING_FILTER_MED_HPP
#define PIC_FILTERING_FILTER_MED_HPP

#include "filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterMed class
 */
class FilterMed: public Filter
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
        int i, j, k, l, ch, c, c2;
        int ci, cj;

        float *data = src[0]->data;
        int areaKernel = (halfSize * 2 + 1) * (halfSize * 2 + 1);
        float *values = new float[areaKernel];

        int channels = dst->channels;

        for(j = box->y0; j < box->y1; j++) {
            for(i = box->x0; i < box->x1; i++) {
                //Central weight
                c = (j * width + i) * channels;

                for(ch = 0; ch < channels; ch++) {
                    c2 = 0;

                    for(k = -halfSize; k <= halfSize; k++) {
                        ci = CLAMP((i + k), dst->width);

                        for(l = -halfSize; l <= halfSize; l++) {
                            cj = CLAMP((j + l), dst->height);
                            values[c2] = data[(cj * width + ci) * channels + ch];
                            c2++;
                        }
                    }

                    std::sort(values, values + areaKernel);
                    dst->data[c + ch] = values[areaKernel >> 1];
                }
            }
        }

        delete[] values;
    }

public:
    /**
     * @brief FilterMed
     * @param size
     */
    FilterMed(int size)
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
        FilterMed filter(size);
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

#endif /* PIC_FILTERING_FILTER_MED_HPP */

