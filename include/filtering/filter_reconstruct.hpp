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

#ifndef PIC_FILTERING_FILTER_RECONSTRUCT_HPP
#define PIC_FILTERING_FILTER_RECONSTRUCT_HPP

#include "../util/array.hpp"

#include "../filtering/filter.hpp"

namespace pic {

class FilterReconstruct: public Filter
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

        for(int j = box->y0; j < box->y1; j++) {

            for(int i = box->x0; i < box->x1; i++) {
                float *tmp_ann = (*src[1])(i, j);
                int x = int(tmp_ann[0]);
                int y = int(tmp_ann[1]);

                float *tmp_dst = (*dst)(i, j);
                float *tmp_src = (*src[0])(x, y);

                Arrayf::assign(tmp_src, channels, tmp_dst);
            }
        }
    }

public:
    /**
     * @brief FilterReconstruct
     */
    FilterReconstruct() : Filter()
    {
        minInputImages = 2;
    }

    /**
     * @brief OutputSize
     * @param imgIn
     * @param width
     * @param height
     * @param channels
     * @param frames
     */
    virtual void OutputSize(ImageVec imgIn, int &width, int &height, int &channels, int &frames)
    {
        width       = imgIn[1]->width;
        height      = imgIn[1]->height;
        channels    = imgIn[1]->channels;
        frames      = imgIn[1]->frames;
    }

    /**
     * @brief execute
     * @param imgIn
     * @param ann
     * @param imgOut
     * @return
     */
    static Image *execute(Image *imgIn, Image *ann, Image *imgOut = NULL)
    {
        FilterReconstruct fltRec;
        return fltRec.Process(Double(imgIn, ann), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_RECONSTRUCT_HPP */
