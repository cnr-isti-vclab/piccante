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

#ifndef PIC_FILTERING_FILTER_LUMINANCE_HPP
#define PIC_FILTERING_FILTER_LUMINANCE_HPP

#include "../util/array.hpp"
#include "../util/std_util.hpp"

#include "../filtering/filter.hpp"

namespace pic {

enum LUMINANCE_TYPE{LT_CIE_LUMINANCE, LT_LUMA, LT_WARD_LUMINANCE, LT_MEAN};

/**
 * @brief The FilterLuminance class
 */
class FilterLuminance: public Filter
{
protected:

    LUMINANCE_TYPE type;
    float *weights;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box)
    {
        for(int j = box->y0; j < box->y1; j++) {

            for(int i = box->x0; i < box->x1; i++) {

                float *data_src = (*src[0])(i, j);
                float *data_dst = (*dst)(i, j);

                data_dst[0] = Arrayf::dot(data_src, weights, src[0]->channels);
            }
        }
    }

public:

    /**
     * @brief FilterLuminance
     * @param type
     */
    FilterLuminance(LUMINANCE_TYPE type = LT_CIE_LUMINANCE) : Filter()
    {
        weights = NULL;
        update(type);
    }

    ~FilterLuminance()
    {
        weights = delete_s(weights);
    }

    /**
     * @brief computeWeights
     * @param type
     * @param weights
     * @return
     */
    static float *computeWeights(LUMINANCE_TYPE type, int channels, float *weights)
    {
        if(weights == NULL) {
            weights = new float[channels];
        }

        if(channels == 3) {
            switch(type)
            {
            case LT_WARD_LUMINANCE:
             {
                weights[0] =  54.0f  / 256.0f;
                weights[1] =  183.0f / 256.0f;
                weights[2] =  19.0f  / 256.0f;
            } break;

            case LT_LUMA:
            {
                weights[0] =  0.2989f;
                weights[1] =  0.5870f;
                weights[2] =  0.114f;
            } break;

            case LT_CIE_LUMINANCE:
            {
                weights[0] =  0.2126f;
                weights[1] =  0.7152f;
                weights[2] =  0.0722f;
            } break;

            default:
            {
                weights[0] = 1.0f / 3.0f;
                weights[1] = weights[0];
                weights[2] = weights[0];
            }
            }
        } else {
            if(channels == 1) {
                weights[0] = 1.0f;
            } else {
                Arrayf::assign(1.0f / float(channels), weights, channels);
            }
        }

        return weights;
    }

    /**
     * @brief update
     * @param type
     */
    void update(LUMINANCE_TYPE type = LT_CIE_LUMINANCE)
    {
        this->type = type;
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
        width       = imgIn[0]->width;
        height      = imgIn[0]->height;
        channels    = 1;
        frames      = imgIn[0]->frames;

        weights = delete_s(weights);
        weights = computeWeights(type, imgIn[0]->channels, weights);
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param type
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut, LUMINANCE_TYPE type = LT_CIE_LUMINANCE)
    {
        FilterLuminance fltLum(type);
        return fltLum.Process(Single(imgIn), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_LUMINANCE_HPP */

