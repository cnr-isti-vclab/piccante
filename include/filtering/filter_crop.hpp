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

#ifndef PIC_FILTERING_FILTER_CROP_HPP
#define PIC_FILTERING_FILTER_CROP_HPP

#include "../filtering/filter.hpp"
#include "../util/vec.hpp"

namespace pic {

/**
 * @brief The FilterCrop class
 */
class FilterCrop: public Filter
{
protected:
    bool flag;
    Vec4i mini, maxi;
    Vec3f minf, maxf;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box);

public:

    /**
     * @brief FilterCrop
     * @param min
     * @param max
     */
    FilterCrop(Vec2i min, Vec2i max);

    /**
     * @brief FilterCrop
     * @param min
     * @param max
     */
    FilterCrop(Vec3i min, Vec3i max);

    /**
     * @brief FilterCrop
     * @param min
     * @param max
     */
    FilterCrop(Vec4i min, Vec4i max);

    /**
     * @brief FilterCrop
     * @param min
     * @param max
     */
    FilterCrop(Vec3f min, Vec3f max);

    /**
     * @brief OutputSize
     * @param imgIn
     * @param width
     * @param height
     * @param channels
     * @param frames
     */
    void OutputSize(ImageVec imgIn, int &width, int &height, int &channels, int &frames);

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param min
     * @param max
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut, Vec4i min,
                             Vec4i max)
    {
        FilterCrop fltCrop(min, max);
        return fltCrop.Process(Single(imgIn), imgOut);
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param min
     * @param max
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut, Vec2i min,
                             Vec2i max)
    {
        FilterCrop fltCrop(min, max);
        return fltCrop.Process(Single(imgIn), imgOut);
    }

    /**
     * @brief test
     */
    static void test()
    {
        Image img(1, 512, 512, 3);
        img = 1.0f;

        FilterCrop flt(Vec2i(100, 100), Vec2i(200, 200));

        Image *out = flt.Process(Single(&img), NULL);

        out->Write("test_crop_2d_output.png");
    }
};

PIC_INLINE FilterCrop::FilterCrop(Vec2i min, Vec2i max) : Filter()
{
    mini[0] = min[0];
    mini[1] = min[1];
    mini[2] = 0;
    mini[3] = 0;

    maxi[0] = max[0];
    maxi[1] = max[1];
    maxi[2] = 1;
    maxi[3] = INT_MAX;

    flag = false;
}

PIC_INLINE FilterCrop::FilterCrop(Vec3i min, Vec3i max) : Filter()
{
    for(int i = 0; i < 3; i++) {
        this->mini[i] = min[i];
        this->maxi[i] = max[i];
    }

    mini[3] = 0;
    maxi[3] = INT_MAX;

    flag = false;
}

PIC_INLINE FilterCrop::FilterCrop(Vec4i min, Vec4i max) : Filter()
{
    this->mini = min;
    this->maxi = max;

    flag = false;
}

PIC_INLINE FilterCrop::FilterCrop(Vec3f min, Vec3f max) : Filter()
{
    this->minf = min;
    this->maxf = max;

    flag = true;
}

PIC_INLINE void FilterCrop::OutputSize(ImageVec imgIn, int &width, int &height, int &channels, int &frames)
{
    if(flag) {
        mini[0] = int(minf[0] * imgIn[0]->widthf);
        mini[1] = int(minf[1] * imgIn[0]->heightf);
        mini[2] = int(minf[2] * imgIn[0]->framesf);

        maxi[0] = int(maxf[0] * imgIn[0]->widthf);
        maxi[1] = int(maxf[1] * imgIn[0]->heightf);
        maxi[2] = int(maxf[2] * imgIn[0]->framesf);
    }

    channels = MIN(imgIn[0]->channels, maxi[3]) - mini[3];

    if(mini[3] > 0) {
        channels++;
    }

    int delta[3];
    for(int i = 0; i < 3; i++) {
        delta[i] = maxi[i] - mini[i];
    }

    if(delta[0] <= 0) {
        delta[0] = imgIn[0]->width;
        mini[0]  = 0;
        maxi[0]  = imgIn[0]->width;
    }

    if(delta[1] <= 0) {
        delta[1] = imgIn[0]->height;
        mini[1]  = 0;
        maxi[1]  = imgIn[0]->height;
    }

    if(delta[2] <= 0) {
        delta[2] = imgIn[0]->frames;
        mini[2]  = 0;
        maxi[2]  = imgIn[0]->frames;
    }

    width = delta[0];
    height = delta[1];
    frames = delta[2];
}

PIC_INLINE void FilterCrop::ProcessBBox(Image *dst, ImageVec src, BBox *box)
{
    maxi[3] = MIN(maxi[3], src[0]->channels);

    for(int p = box->z0; p < box->z1; p++) {
        for(int j = box->y0; j < box->y1; j++) {
            for(int i = box->x0; i < box->x1; i++) {
                float *dst_data = (*dst)(i - mini[0], j - mini[1], p - mini[2]);
                float *src_data = (*src[0])(i, j, p);

                for(int k = mini[3]; k <= maxi[3]; k++) {
                    dst_data[k - mini[3]] = src_data[k];
                }
            }
        }
    }
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_CROP_HPP */

