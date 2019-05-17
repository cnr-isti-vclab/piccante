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

#include "../util/array.hpp"

#include "../filtering/filter.hpp"
#include "../filtering/filter_down_pp.hpp"
#include "../image_samplers/image_sampler_bilinear.hpp"

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

                float dist = Arrayf::distanceSq(data, value, src[0]->channels);

                if(dist <= threshold) {
                    isb.SampleImage(src[0], x, y, data);
                }
            }
        }
    }

public:

    /**
     * @brief FilterUpPP
     * @param value
     * @param threshold
     */
    FilterUpPP(float *value, float threshold) : Filter()
    {
        update(value, threshold);
    }

    ~FilterUpPP()
    {
    }

    /**
     * @brief update
     * @param value
     * @param threshold
     */
    void update(float *value, float threshold)
    {
        this->value = value;

        if(value == NULL) {
            printf("ERROR in FilterUpPP");
        }

        this->value = value;

        this->threshold = (threshold > 0.0f) ? threshold : 1e-6f;
    }

    /**
     * @brief OutputSize
     * @param imgIn
     * @param width
     * @param height
     * @param channels
     * @param frames
     */
    void OutputSize(ImageVec imgIn, int &width, int &height, int &channels, int &frames)
    {
        if(imgIn.size() == 1) {
            width       = imgIn[0]->width << 1;
            height      = imgIn[0]->height << 1;
        } else {
            width       = imgIn[1]->width;
            height      = imgIn[1]->height;
        }

        channels    = imgIn[0]->channels;
        frames      = imgIn[0]->frames;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_DOWN_PP_HPP */

