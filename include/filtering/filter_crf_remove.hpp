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

#include "../filtering/filter.hpp"

#include "../algorithms/camera_response_function.hpp"

namespace pic {

/**
 * @brief The FilterCRFRemove class
 */
class FilterCRFRemove: public Filter
{
protected:
    CameraResponseFunction *crf;

    /**
     * @brief f
     * @param data
     */
    virtual void f(FilterFData *data)
    {
        float *tmp_src = (*data->src[0])(data->x, data->y);

        for(int k = 0; k < data->src[0]->channels; k++) {
            data->out[k] = crf->Remove(tmp_src[k], k);
        }
    }

public:

    /**
     * @brief FilterCRFRemove
     */
    FilterCRFRemove(CameraResponseFunction *crf)
    {
        if(crf != NULL) {
            this->crf = crf;
        } else {
            printf("FilterCRFRemove: the crf is NULL!\n");
        }
    }

    /**
     * @brief execute
     * @param imgIn1
     * @param imgIn2
     * @return
     */
    static Image *execute(Image *imgIn, CameraResponseFunction *crf)
    {
        FilterCRFRemove filter(crf);
        return filter.Process(Single(imgIn), NULL);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_CRF_REMOVE_HPP */

