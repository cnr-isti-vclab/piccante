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

#include "../filtering/filter.hpp"

namespace pic {

enum LUMINANCE_TYPE{LT_CIE_LUMINANCE, LT_WARD_LUMINANCE, LT_MEAN, LT_PASS_THROUGH};

/**
 * @brief The FilterLuminance class
 */
class FilterLuminance: public Filter
{
protected:

    LUMINANCE_TYPE type;
    float *weights;
    int weights_size;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box)
    {
        int transformChannels = MIN(src[0]->channels, weights_size);

        for(int j = box->y0; j < box->y1; j++) {

            for(int i = box->x0; i < box->x1; i++) {

                float *data_src = (*src[0])(i, j);
                float *data_dst = (*dst)(i, j);

                data_dst[0] = 0.0f;
                for(int k = 0; k < transformChannels; k++) {
                    data_dst[0] += data_src[k] * weights[k];
                }
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
        weights_size = -1;

        update(type);
    }

    ~FilterLuminance()
    {
        if(weights != NULL) {
            delete[] weights;
        }
    }

    /**
     * @brief update
     * @param type
     */
    void update(LUMINANCE_TYPE type = LT_CIE_LUMINANCE)
    {
        this->type = type;

        if(weights != NULL) {
            delete[] weights;
        }

        weights = NULL;

        switch(type)
        {
        case LT_WARD_LUMINANCE:
            {
                weights = new float[3];
                weights_size = 3;
                weights[0] =  54.0f  / 256.0f;
                weights[1] =  183.0f / 256.0f;
                weights[2] =  19.0f  / 256.0f;
            }
            break;

        case LT_CIE_LUMINANCE:
            {
                weights = new float[3];
                weights_size = 3;
                weights[0] =  0.2126f;
                weights[1] =  0.7152f;
                weights[2] =  0.0722f;
            }
            break;

        case LT_MEAN:
            {
                weights = NULL;
                weights_size = -1;
            }
            break;
                
        default:
            {
                weights = NULL;
                weights_size = -1;
            }
            break;
        }
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

        bool bChannels = (weights_size != imgIn[0]->channels);
        if( bChannels && (type == LT_MEAN) ) {
            weights_size = imgIn[0]->channels;

            if(weights != NULL) {
                delete[] weights;
            }

            weights = new float [weights_size];
            Arrayf::assign(1.0f / imgIn[0]->channelsf, weights, imgIn[0]->channels);
        } else {
            if(bChannels) {
                weights = new float [1];
                weights[0] = 1.0f;

                type = LT_PASS_THROUGH;
            }
        }
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

    /**
     * @brief execute
     * @param fileInput
     * @param fileOutput
     * @return
     */
    static Image *execute(std::string fileInput, std::string fileOutput)
    {
        Image imgIn(fileInput);
        Image *out = FilterLuminance::execute(&imgIn, NULL, LT_CIE_LUMINANCE);
        out->Write(fileOutput);
        return out;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_LUMINANCE_HPP */

