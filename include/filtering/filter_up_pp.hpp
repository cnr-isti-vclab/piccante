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

#ifndef PIC_FILTERING_FILTER_UP_PP_HPP
#define PIC_FILTERING_FILTER_UP_PP_HPP

#include "filtering/filter.hpp"
#include "filtering/filter_down_pp.hpp"
#include "image_samplers/image_sampler_bilinear.hpp"

namespace pic {

/**
 * @brief The FilterUpPP class
 */
class FilterUpPP: public Filter
{
protected:

    ImageSamplerBilinear isb;

    float *value, threshold;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box)
    {
        for(int i = box->y0; i < box->y1; i++) {
            float y = float(i) / dst->heightf;

            for(int j = box->x0; j < box->x1; j++) {
                float x = float(j) / dst->widthf;

                float *data = (*dst)(j, i);

                if(FilterDownPP::distance(data, value, dst->channels) < threshold) {
                    isb.SampleImage(src[0], x, y, data);
                }
            }
        }
    }

    /**
     * @brief SetupAux
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *SetupAux(ImageVec imgIn, Image *imgOut)
    {
        if(imgOut == NULL) {
            imgOut = new Image(1, imgIn[0]->width << 1, imgIn[0]->height << 1, imgIn[0]->channels);
        }

        return imgOut;
    }

public:

    /**
     * @brief FilterUpPP
     * @param value
     * @param threshold
     */
    FilterUpPP(float *value, float threshold)
    {
        Update(value, threshold);
    }

    ~FilterUpPP()
    {
    }

    /**
     * @brief Update
     * @param value
     * @param threshold
     */
    void Update(float *value, float threshold)
    {
        if(value == NULL) {
            this->value = new float[3];

            for(int i = 0; i < 3; i++) {
                this->value[i] = 0.0f;
            }
        } else {
            this->value = value;
        }

        if(threshold > 0.0f) {
            this->threshold = threshold;
        } else {
            this->threshold = 1e-4f;
        }
    }

    /**
     * @brief Execute
     * @param imgIn
     * @param imgOut
     * @param type
     * @return
     */
    static Image *Execute(Image *imgIn, Image *imgOut)
    {
        FilterUpPP flt(NULL, 1e-6f);
        return flt.ProcessP(Single(imgIn), imgOut);
    }

    /**
     * @brief Execute
     * @param fileInput
     * @param fileOutput
     * @return
     */
    static Image *Execute(std::string fileInput, std::string fileOutput)
    {
        Image imgIn(fileInput);
        Image *out = FilterUpPP::Execute(&imgIn, NULL);
        out->Write(fileOutput);
        return out;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_DOWN_PP_HPP */

