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

#ifndef PIC_FILTERING_FILTER_DOWN_PP_HPP
#define PIC_FILTERING_FILTER_DOWN_PP_HPP

#include "../util/array.hpp"

#include "../filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterDownPP class
 */
class FilterDownPP: public Filter
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
        int channels = src[0]->channels;

        for(int i2 = box->y0; i2 < box->y1; i2++) {
            int i = i2 << 1;

            for(int j2 = box->x0; j2 < box->x1; j2++) {
                int j = j2 << 1;

                float *tmp[4];
                tmp[0] = (*src[0])(j    , i);
                tmp[1] = (*src[0])(j + 1, i);
                tmp[2] = (*src[0])(j    , i + 1);
                tmp[3] = (*src[0])(j + 1, i + 1);

                int counter = 0;
                float *out = (*dst)(j2, i2);

                Arrayf::assign(0.0f, out, channels);

                for(int k = 0; k < 4; k++) {
                    if(Arrayf::distanceSq(tmp[k], value, channels) > threshold) {
                        counter++;

                        for(int l = 0; l < channels; l++) {
                            out[l] += tmp[k][l];
                        }
                    }
                }

                if(counter > 0) {
                    float counter_f = float(counter);                    
                    Arrayf::div(out, channels, counter_f);
                } else {
                    Arrayf::assign(value, channels, out);
                }
            }
        }
    }

    float *value, threshold;

public:

    /**
     * @brief FilterDownPP
     * @param value
     * @param threshold
     */
    FilterDownPP(float *value, float threshold) : Filter()
    {
        update(value, threshold);
    }

    ~FilterDownPP()
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
            printf("ERROR in FilterDownPP");
        }

        this->threshold = (threshold > 0.0f) ? threshold : 1e-4f;
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
            width       = imgIn[0]->width >> 1;
            height      = imgIn[0]->height >> 1;
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

