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

#ifndef PIC_FILTERING_FILTER_MOSAIC_HPP
#define PIC_FILTERING_FILTER_MOSAIC_HPP

#include "../filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterMosaic class
 */
class FilterMosaic: public Filter
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
        if(src[0]->channels != 3){
            return;
        }

        int width = dst->width;
        float *data = src[0]->data;

        for(int j = box->y0; j < box->y1; j++) {
            int c = j * width;
            int mody = j % 2;

            for(int i = box->x0; i < box->x1; i++) {
                int modx = i % 2;

                //indecies
                int c1 = c + i;
                int c3 = c1 * 3;

                if(mody == 0 && modx == 0) { //Red
                    dst->data[c1] = data[c3];
                }

                if(mody == 0 && modx == 1) { //Green
                    dst->data[c1] = data[c3 + 1];
                }

                if(mody == 1 && modx == 0) { //Green
                    dst->data[c1] = data[c3 + 1];
                }

                if(mody == 1 && modx == 1) { //Blue
                    dst->data[c1] = data[c3 + 2];
                }
            }
        }
    }

public:
    /**
     * @brief FilterMosaic
     */
    FilterMosaic() : Filter()
    {

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
        width    = imgIn[0]->width;
        height   = imgIn[0]->height;
        channels = 1;
        frames   = imgIn[0]->frames;
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut)
    {
        FilterMosaic flt;
        return flt.Process(Single(imgIn), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_MOSAIC_HPP */

