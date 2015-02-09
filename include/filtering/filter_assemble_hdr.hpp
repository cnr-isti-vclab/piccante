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

#ifndef PIC_FILTERING_FILTER_ASSEMBLE_HDR_HPP
#define PIC_FILTERING_FILTER_ASSEMBLE_HDR_HPP

#include "filtering/filter.hpp"

#include "algorithms/camera_response_function.hpp"

namespace pic {

/**
 * @brief The FilterAssembleHDR class
 */
class FilterAssembleHDR: public Filter
{
protected:
    CRF_WEIGHT              weight_type;
    IMG_LIN                 linearization_type;
    std::vector<float *>    *icrf;

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

        unsigned int n = src.size();

        bool bFunction = (icrf != NULL) && (linearization_type == LIN_ICFR);

        float maxVal = FLT_MAX;
        for(int j = 0; j < n; j++) {
            maxVal = MIN(maxVal, src[j]->exposure);
        }
        maxVal = 1.0f / maxVal;

        for(int j = box->y0; j < box->y1; j++) {
            int ind = j * width;

            for(int i = box->x0; i < box->x1; i++) {

                //Assembling kernel
                int c = (ind + i) * channels;

                bool flag = false;

                for(int k = 0; k < channels; k++) {
                    float weight_norm = 0.0f;
                    float acc = 0.0f;

                    for(unsigned int l = 0; l < n; l++) {
                        float x = src[l]->data[c + k];

                        float weight = WeightFunction(x, weight_type);

                        float x_lin;
                        if(bFunction) {
                            x_lin = Linearize(x, linearization_type, icrf->at(k));
                        } else {
                            x_lin = x;
                        }

                        acc += (weight * x_lin) / src[l]->exposure;

                        weight_norm += weight;
                    }

                    flag = flag || (weight_norm <= 0.0f);

                    float final_value = weight_norm > 0.0f ? (acc / weight_norm) : maxVal;
                    dst->data[c + k] = final_value;
                }
            }
        }
    }

public:

    /**
     * @brief FilterAssembleHDR
     * @param weight_type
     * @param linearization_type
     * @param icrf
     */
    FilterAssembleHDR(CRF_WEIGHT weight_type = CW_GAUSS, IMG_LIN linearization_type = LIN_LIN, std::vector<float *> *icrf = NULL)
    {
        this->weight_type = weight_type;

        this->linearization_type = linearization_type;

        this->icrf = icrf;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_ASSEMBLE_HDR_HPP */

