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

#ifndef PIC_FILTERING_FILTER_LAPLACIAN_HPP
#define PIC_FILTERING_FILTER_LAPLACIAN_HPP

#include "filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterLaplacian class
 */
class FilterLaplacian: public Filter
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
        int width = dst->width;
        int height = dst->height;
        int channels = src[0]->channels;
        float *data = src[0]->data;

        float sum;
        int tmp[4], cip1, cjp1, cim1, cjm1;

        for(int j = box->y0; j < box->y1; j++) {
            int ind = j * width;

            for(int i = box->x0; i < box->x1; i++) {
                int c = (ind + i) * channels;

                //neighbors
                tmp[0]  = CLAMP(i + 1, width);
                cip1    = (ind + tmp[0]) * channels;

                tmp[1]  = CLAMP(i - 1, width);
                cim1    = (ind + tmp[1]) * channels;

                tmp[2]  = CLAMP(j + 1, height);
                cjp1    = (tmp[2] * width + i) * channels;

                tmp[3]  = CLAMP(j - 1, height);
                cjm1    = (tmp[3] * width + i) * channels;

                for(int k = 0; k < channels; k++) {
                    sum  = -4.0f * data[c + k];
                    sum += data[cip1 + k];
                    sum += data[cim1 + k];
                    sum += data[cjp1 + k];
                    sum += data[cjm1 + k];

                    dst->data[c + k] = sum;
                }
            }
        }
    }

public:
    /**
     * @brief FilterLaplacian
     */
    FilterLaplacian()
    {

    }

    /**
     * @brief Execute
     * @param imgIn
     * @param imgOut
     * @return
     */
    static Image *Execute(Image *imgIn, Image *imgOut)
    {
        FilterLaplacian filter;
        return filter.ProcessP(Single(imgIn), imgOut);
    }

    /**
     * @brief Execute
     * @param nameIn
     * @param nameOut
     */
    static void Execute(std::string nameIn, std::string nameOut)
    {
        Image imgIn(nameIn);
        Image *imgOut = Execute(&imgIn, NULL);
        imgOut->Write(nameOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_LAPLACIAN_HPP */

