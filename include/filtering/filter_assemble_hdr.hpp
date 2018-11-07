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

#include "../filtering/filter.hpp"

#include "../util/array.hpp"

#include "../algorithms/camera_response_function.hpp"

namespace pic {

/**
 * @brief The HDR_REC_DOMAIN enum
 * HRD_LOG: assembling HDR image in the log-domain

 * HRD_LIN: assembling HDR image in the linear domain
 *
 * HRD_SQ: assembling HDR image in the linear domain with t^2 trick
 * for reducing noise [Robertson et al.]
 */
enum HDR_REC_DOMAIN {HRD_LOG, HRD_LIN, HRD_SQ};

/**
 * @brief The FilterAssembleHDR class
 */
class FilterAssembleHDR: public Filter
{
protected:
    CameraResponseFunction *crf;
    HDR_REC_DOMAIN          domain;
    CRF_WEIGHT              weight_type;
    float                   delta_value;

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

        int n = int(src.size());

        float t_min = src[0]->exposure;
        int index = 0;
        for(int j = 1; j < n; j++) {
            if(src[j]->exposure < t_min) {
                t_min = src[j]->exposure;
                index = j;
            }
        }        

        float channelsf = float(channels);

        float *acc = new float[channels];
        float *totWeight = new float[channels];

        for(int j = box->y0; j < box->y1; j++) {
            int ind = j * width;

            for(int i = box->x0; i < box->x1; i++) {
                int c = (ind + i) * channels;

                Arrayf::assign(0.0f, acc, channels);
                Arrayf::assign(0.0f, totWeight, channels);

                float max_val_saturation = 1.0f;
                float max_val_saturation_fb = -1.0f;

                //for each exposure...
                for(int l = 0; l < n; l++) {

                    float x = Arrayf::sum(&src[l]->data[c], channels);
                    x /= channelsf;

                    float t_mvs = x / t_min;

                    max_val_saturation_fb = MAX(max_val_saturation_fb, t_mvs);

                    if(l == index) {
                        max_val_saturation = t_mvs;
                    }

                    float weight = weightFunction(x, weight_type);

                    if(domain == HRD_SQ) {
                        weight *= (src[l]->exposure * src[l]->exposure);
                    }

                    for(int k = 0; k < channels; k++) {
                        float x_lin = crf->remove(src[l]->data[c + k], k);

                        //merge HDR pixels
                        switch(domain) {
                            case HRD_LIN: {
                                acc[k] += (weight * x_lin) / src[l]->exposure;
                            } break;

                            case HRD_LOG: {
                                acc[k] += weight * (logf(x_lin + delta_value) - logf(src[l]->exposure));
                            } break;

                            case HRD_SQ: {
                                acc[k] += (weight * x_lin) * src[l]->exposure;
                            } break;
                        }

                        totWeight[k] += weight;
                    }
                }

                bool bSaturated = false;
                for(int k = 0; k < channels; k++) {
                    bSaturated = bSaturated || (totWeight[k] < 1e-4f);
                }

                if(!bSaturated) {
                    for(int k = 0; k < channels; k++) {
                        acc[k] /= totWeight[k];
                        if(domain == HRD_LOG) {
                            acc[k] = expf(acc[k]);
                        }
                        dst->data[c + k] = acc[k];
                    }
                } else {
                    max_val_saturation = MAX(max_val_saturation_fb, max_val_saturation);
                    Arrayf::assign(max_val_saturation, &dst->data[c], channels);
                }
            }
        }

        delete[] totWeight;
        delete[] acc;
    }

public:

    /**
     * @brief FilterAssembleHDR
     * @param weight_type
     * @param linearization_type
     * @param icrf
     */
    FilterAssembleHDR(CameraResponseFunction *crf, CRF_WEIGHT weight_type = CW_DEB97, HDR_REC_DOMAIN domain = HRD_LOG) : Filter()
    {        
        this->crf = crf;

        this->weight_type = weight_type;

        this->domain = domain;

        //a numerical stability value when assembling images in the log-domain
        this->delta_value = 1.0 / 65536.0f;

        minInputImages = 2;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_ASSEMBLE_HDR_HPP */

