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

#ifndef PIC_FILTERING_FILTER_ABSOLUTE_DIFFERENCE_HPP
#define PIC_FILTERING_FILTER_ABSOLUTE_DIFFERENCE_HPP

#include "filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterAbsoluteDifference class
 */
class FilterAbsoluteDifference: public Filter
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
        if(src.size()!=2) {
            return;
        }

        int channels = dst->channels;

        for(int i = box->y0; i < box->y1; i++) {
           for(int j = box->x0; j < box->x1; j++) {

                float *tmp_src0 = (*src[0])(j, i);
                float *tmp_src1 = (*src[1])(j, i);
                float *tmp_dst  = (*dst   )(j, i);

                for(int k = 0; k < channels; k++) {
                    tmp_dst[k] = fabsf(tmp_src0[k] - tmp_src1[k]);
                }
            }
        }
    }

public:

    /**
     * @brief FilterAbsoluteDifference
     */
    FilterAbsoluteDifference() {}

    /**
     * @brief Execute
     * @param imgIn1
     * @param imgIn2
     * @return
     */
    static Image *Execute(Image *imgIn1, Image *imgIn2)
    {
        FilterAbsoluteDifference filter;
        return filter.Process(Double(imgIn1, imgIn2), NULL);
    }

    /**
     * @brief Execute
     * @param name1
     * @param name2
     * @param nameOut
     * @return
     */
    static Image *Execute(std::string name1, std::string name2,
                             std::string nameOut)
    {
        Image imgIn1(name1);
        Image imgIn2(name2);

        Image *out = Execute(&imgIn1, &imgIn2);
        out->Write(nameOut);
        return out;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_ABSOLUTE_DIFFERENCE_HPP */

