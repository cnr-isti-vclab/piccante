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

#ifndef PIC_FILTERING_FILTER_CRF_REMOVE_HPP
#define PIC_FILTERING_FILTER_CRF_REMOVE_HPP

#include "filtering/filter.hpp"

#include "algorithms/camera_response_function.hpp"

namespace pic {

/**
 * @brief The FilterCRFRemove class
 */
class FilterCRFRemove: public Filter
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

        if(type == IL_LUT_8_BIT && crf->icrf.size() != src[0]->channels) {
            #ifdef PIC_DEBUG
                printf("Warning: img cannot be linearized.\n");
            #endif
            return;
        }

        int channels = dst->channels;

        for(int i = box->y0; i < box->y1; i++) {
           for(int j = box->x0; j < box->x1; j++) {

                float *tmp_src = (*src[0])(j, i);
                float *tmp_dst = (*dst   )(j, i);

                for(int k = 0; k < channels; k++) {
                     tmp_dst[k] = crf->Remove(tmp_src[k], k);
                }
            }
        }
    }

    CameraResponseFunction *crf;

public:

    /**
     * @brief FilterCRFRemove
     */
    FilterCRFRemove(CameraResponseFunction *crf)
    {
        this->crf = crf;
    }

    /**
     * @brief Execute
     * @param imgIn1
     * @param imgIn2
     * @return
     */
    static Image *Execute(Image *imgIn, CameraResponseFunction *crf)
    {
        FilterCRFRemove filter(crf);
        return filter.Process(Single(imgIn), NULL);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_CRF_REMOVE_HPP */

