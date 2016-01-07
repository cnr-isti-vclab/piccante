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

enum HDR_REC_DOMAIN {HRD_LOG, HRD_LIN};

/**
 * @brief The FilterAssembleHDR class
 */
class FilterAssembleHDR: public Filter
{
protected:
    HDR_REC_DOMAIN          domain;
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

        bool bFunction = (icrf != NULL) && (linearization_type == IL_LUT_8_BIT);

        float t_min = FLT_MAX;
        int index = -1;
        for(unsigned int j = 0; j < n; j++) {
            if(src[j]->exposure < t_min) {
                t_min = src[j]->exposure;
                index = j;
            }
        }        
        float max_val_saturation = 1.0f / t_min;

        for(int j = box->y0; j < box->y1; j++) {
            int ind = j * width;

            for(int i = box->x0; i < box->x1; i++) {

                //Assembling kernel
                int c = (ind + i) * channels;

                for(int k = 0; k < channels; k++) {
                    float weight_norm = 0.0f;
                    float acc = 0.0f;

                    float *tmp_icrf = NULL;
                    if(icrf != NULL) {
                        tmp_icrf = icrf->at(k);
                    }

                    for(unsigned int l = 0; l < n; l++) {
                        float x = src[l]->data[c + k];

                        float weight = WeightFunction(x, weight_type);

                        float x_lin;

                        if(bFunction) {
                            x_lin = Linearize(x, linearization_type, tmp_icrf);
                        } else {
                            x_lin = x;
                        }

                        switch(domain) {
                            case HRD_LIN: {
                                acc += (weight * x_lin) / src[l]->exposure;
                            } break;

                            case HRD_LOG: {
                                acc += weight * (logf(x_lin) - logf(src[l]->exposure));
                            } break;
                        }

                        weight_norm += weight;
                    }

                    float val;
                    if(weight_norm > 1e-4f) {
                        val = acc / weight_norm;
                        if(domain == HRD_LOG) {
                            val = expf(val);
                        }
                    } else {
                        if(src[index]->data[c + k] < 0.5f) {
                            val = 0.0f;
                        }

                        if(src[index]->data[c + k] > 0.9f) {
                            val = max_val_saturation;
                        }
                    }

                    dst->data[c + k] = val;
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
    FilterAssembleHDR(CRF_WEIGHT weight_type = CW_GAUSS, HDR_REC_DOMAIN domain = HRD_LOG,
                      IMG_LIN linearization_type = IL_LIN, std::vector<float *> *icrf = NULL)
    {
        this->domain = domain;

        this->weight_type = weight_type;

        this->linearization_type = linearization_type;

        this->icrf = icrf;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_ASSEMBLE_HDR_HPP */

