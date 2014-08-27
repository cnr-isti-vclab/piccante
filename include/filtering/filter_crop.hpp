/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

PICCANTE is free software; you can redistribute it and/or modify
under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 3.0 of
the License, or (at your option) any later version.

PICCANTE is distributed in the hope that it will be useful, but
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License
( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.

*/

#ifndef PIC_FILTERING_FILTER_CROP_HPP
#define PIC_FILTERING_FILTER_CROP_HPP

#include "filtering/filter.hpp"
#include "util/vec.hpp"

namespace pic {

/**
 * @brief The FilterCrop class
 */
class FilterCrop: public Filter
{
protected:
    bool			flag;
    Vec<4, int>		maxi, mini;
    Vec<3, float>	maxf, minf;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(ImageRAW *dst, ImageRAWVec src, BBox *box);

public:

    /**
     * @brief FilterCrop
     * @param min
     * @param max
     */
    FilterCrop(Vec<2, int>   min, Vec<2, int>   max);

    /**
     * @brief FilterCrop
     * @param min
     * @param max
     */
    FilterCrop(Vec<3, int>   min, Vec<3, int>   max);

    /**
     * @brief FilterCrop
     * @param min
     * @param max
     */
    FilterCrop(Vec<4, int>   min, Vec<4, int>   max);

    /**
     * @brief FilterCrop
     * @param min
     * @param max
     */
    FilterCrop(Vec<3, float> min, Vec<3, float> max);

    /**
     * @brief SetupAux
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageRAW *SetupAux(ImageRAWVec imgIn, ImageRAW *imgOut);

    /**
     * @brief Execute
     * @param imgIn
     * @param imgOut
     * @param min
     * @param max
     * @return
     */
    static ImageRAW *Execute(ImageRAW *imgIn, ImageRAW *imgOut, Vec<4, int> min,
                             Vec<4, int> max)
    {
        FilterCrop fltCrop(min, max);
        return fltCrop.Process(Single(imgIn), imgOut);
    }

    /**
     * @brief Execute
     * @param imgIn
     * @param imgOut
     * @param min
     * @param max
     * @return
     */
    static ImageRAW *Execute(ImageRAW *imgIn, ImageRAW *imgOut, Vec<2, int> min,
                             Vec<2, int> max)
    {
        FilterCrop fltCrop(min, max);
        return fltCrop.Process(Single(imgIn), imgOut);
    }

    /**
     * @brief Execute
     * @param fileInput
     * @param fileOutput
     * @param min
     * @param max
     * @return
     */
    static ImageRAW *Execute(std::string fileInput, std::string fileOutput,
                             Vec<2, int> min, Vec<2, int> max)
    {
        ImageRAW imgIn(fileInput);
        ImageRAW *out = FilterCrop::Execute(&imgIn, NULL, min, max);
        out->Write(fileOutput);
        return out;
    }

    /**
     * @brief TestCrop2D
     */
    static void TestCrop2D()
    {
        ImageRAW img(1, 512, 512, 3);
        img.Assign(1.0f);

        FilterCrop flt(Vec<2, int>(100, 100), Vec<2, int>(200, 200));

        ImageRAW *out = flt.Process(Single(&img), NULL);

        out->Write("test_crop_2d_output.pfm");
    }
};

FilterCrop::FilterCrop(Vec<2, int> min, Vec<2, int> max)
{
    maxi[0] = max[0];
    maxi[1] = max[1];
    maxi[2] = 1;
    maxi[3] = INT_MAX;

    mini[0] = min[0];
    mini[1] = min[1];
    mini[2] = 0;
    mini[3] = 0;

    flag = false;
}

FilterCrop::FilterCrop(Vec<3, int> min, Vec<3, int> max)
{
    for(int i = 0; i < 3; i++) {
        this->maxi[i] = max[i];
        this->mini[i] = min[i];
    }

    maxi[3] = INT_MAX;
    mini[3] = 0;

    flag = false;
}

FilterCrop::FilterCrop(Vec<4, int> min, Vec<4, int> max)
{
    this->maxi = max;
    this->mini = min;

    flag = false;
}

FilterCrop::FilterCrop(Vec<3, float> min, Vec<3, float> max)
{
    this->maxf = max;
    this->minf = min;

    flag = true;
}

ImageRAW *FilterCrop::SetupAux(ImageRAWVec imgIn, ImageRAW *imgOut)
{
    if(flag) {
        maxi[0] = int(maxf[0] * imgIn[0]->widthf);
        maxi[1] = int(maxf[1] * imgIn[0]->heightf);
        maxi[2] = int(maxf[2] * imgIn[0]->framesf);

        mini[0] = int(minf[0] * imgIn[0]->widthf);
        mini[1] = int(minf[1] * imgIn[0]->heightf);
        mini[2] = int(minf[2] * imgIn[0]->framesf);
    }

    if(imgOut == NULL) {
        int channels = MIN(imgIn[0]->channels, maxi[3]) - mini[3];

        if(mini[3] > 0) {
            channels++;
        }

        int delta[3];

        for(int i = 0; i < 3; i++) {
            delta[i] = maxi[i] - mini[i];
        }

        if(delta[0] <= 0) {
            delta[0] = imgIn[0]->width;
            maxi[0]  = imgIn[0]->width;
            mini[0]  = 0;
        }

        if(delta[1] <= 0) {
            delta[1] = imgIn[0]->height;
            maxi[1]  = imgIn[0]->height;
            mini[1]  = 0;
        }

        if(delta[2] <= 0) {
            delta[2] = imgIn[0]->frames;
            maxi[2]  = imgIn[0]->frames;
            mini[2]  = 0;
        }

        imgOut = new ImageRAW(delta[2], delta[0], delta[1], channels);
    }

    return imgOut;
}

void FilterCrop::ProcessBBox(ImageRAW *dst, ImageRAWVec src, BBox *box)
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

