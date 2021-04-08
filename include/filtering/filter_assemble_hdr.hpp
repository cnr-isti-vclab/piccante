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
    HDR_REC_DOMAIN domain;
    CRF_WEIGHT weight_type;
    float delta_value;

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
        int i_min = 0;

        float t_max = src[0]->exposure;
        int i_max = 0;

        for(int j = 1; j < n; j++) {
            if(src[j]->exposure < t_min) {
                t_min = src[j]->exposure;
                i_min = j;
            }
            
            if(src[j]->exposure > t_max) {
                t_max = src[j]->exposure;
                i_max = j;
            }
        }
        
        //check i_min
        bool bMin = false;
        Image *img_min = src[i_min];
        for(int i = 0; i < img_min->size(); i++) {
            if(img_min->data[i] > 0.9f) {
                bMin = true;
                break;
            }
        }

        //check i_max
        bool bMax = false;
        Image *img_max = src[i_max];
        for(int i = 0; i < img_max->size(); i++) {
            if(img_max->data[i] < 0.1f) {
                bMax = true;
                break;
            }
        }

        CRF_WEIGHT *weight_type_arr = new CRF_WEIGHT[n];
        for(int l = 0; l < n; l++) {

            weight_type_arr[l] = weight_type;

            if((l == i_min) && bMin) {
                weight_type_arr[l] = CW_IDENTITY;
            }

            if((l == i_max) && bMax) {
                weight_type_arr[l] = CW_REVERSE;
            }
        }

        float *acc = new float[channels];
        float *totWeight = new float[channels];

        for(int j = box->y0; j < box->y1; j++) {
            int ind = j * width;

            for(int i = box->x0; i < box->x1; i++) {
                int c = (ind + i) * channels;

                Arrayf::assign(0.0f, acc, channels);
                Arrayf::assign(0.0f, totWeight, channels);

                //for each exposure...

                for(int l = 0; l < n; l++) {

                    float x = Arrayf::sum(&src[l]->data[c], channels) / dst->channelsf;
                    
                    float weight = weightFunction(x, weight_type_arr[l]);

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

                for(int k = 0; k < channels; k++) {
                    acc[k] /= totWeight[k];
                    if(domain == HRD_LOG) {
                        acc[k] = expf(acc[k]);
                    }
                    dst->data[c + k] = acc[k];
                }
            }
        }

        delete[] weight_type_arr;
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
    FilterAssembleHDR(CameraResponseFunction *crf = NULL, CRF_WEIGHT weight_type = CW_DEB97, HDR_REC_DOMAIN domain = HRD_LOG)
    {
        update(crf, weight_type, domain);
        minInputImages = 2;

        //a numerical stability value when assembling images in the log-domain
        this->delta_value = 1.0 / 65535.0f;
    }

    /**
     * @brief update
     * @param crf
     * @param weight_type
     * @param domain
     */
    void update(CameraResponseFunction *crf, CRF_WEIGHT weight_type = CW_DEB97, HDR_REC_DOMAIN domain = HRD_LOG)
    {        
        this->crf = crf;

        this->weight_type = weight_type;

        this->domain = domain;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_ASSEMBLE_HDR_HPP */

