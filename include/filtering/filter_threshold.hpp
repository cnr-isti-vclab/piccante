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

#ifndef PIC_FILTERING_FILTER_THRESHOLD_HPP
#define PIC_FILTERING_FILTER_THRESHOLD_HPP

#include "../filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterThreshold class
 */
class FilterThreshold: public Filter
{
protected:
    float threshold;
    bool bAdaptive;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box)
    {
        Image* img = src[0];

        if(bAdaptive) {
            if(src.size() < 2) {
                return;
            }

            Image* img_ada = src[1];

            for(int j = box->y0; j < box->y1; j++) {

                for(int i = box->x0; i < box->x1; i++) {
                    float *dst_val = (*dst)(i, j);
                    float *img_val = (*img)(i, j);
                    float *img_ada_val = (*img_ada)(i, j);

                    dst_val[0] = img_val[0] > img_ada_val[0] ? 1.0f : 0.0f;
                }
            }
        } else {
            for(int j = box->y0; j < box->y1; j++) {

                for(int i = box->x0; i < box->x1; i++) {
                    float *dst_val = (*dst)(i, j);
                    float *img_val = (*img)(i, j);

                    dst_val[0] = img_val[0] > threshold ? 1.0f : 0.0f;
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
            imgOut = new Image(1, imgIn[0]->width, imgIn[0]->height, 1);
        } else {
            if((imgIn[0]->width  != imgOut->width)  ||
               (imgIn[0]->height != imgOut->height) ||
               (imgOut->channels != 1)) {
                imgOut = new Image(1, imgIn[0]->width, imgIn[0]->height, 1);
            }
        }

        return imgOut;
    }

public:

    /**
     * @brief FilterThreshold
     * @param threshold
     * @param bAdaptive
     */
    FilterThreshold(float threshold = 0.5f, bool bAdaptive = false) : Filter()
    {
        update(threshold, bAdaptive);
    }

    /**
     * @brief update
     * @param threshold
     * @param bAdaptive
     */
    void update(float threshold, bool bAdaptive)
    {
        this->threshold = threshold;
        this->bAdaptive = bAdaptive;
    }

    /**
     * @brief OutputSize
     * @param imgIn
     * @param width
     * @param height
     * @param channels
     * @param frames
     */
    void OutputSize(Image *imgIn, int &width, int &height, int &channels, int &frames)
    {
        width       = imgIn->width;
        height      = imgIn->height;
        channels    = 1;
        frames      = imgIn->frames;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_THRESHOLD_HPP */

