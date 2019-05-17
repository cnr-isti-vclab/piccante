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

#ifndef PIC_FILTERING_FILTER_KUWAHARA_HPP
#define PIC_FILTERING_FILTER_KUWAHARA_HPP

#include "../filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterKuwahara class
 */
class FilterKuwahara: public Filter
{
protected:
    unsigned int  kernelSize;
    unsigned int  halfKernelSize;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box)
    {
        int channels = dst->channels;

        Image *source = src[0];

        float *buf = new float[channels * 8];

        int start = 0;
        float *m00 = &buf[start];
        start += channels;
        float *m10 = &buf[start];
        start += channels;
        float *m01 = &buf[start];
        start += channels;
        float *m11 = &buf[start];
        start += channels;
        float *s00 = &buf[start];
        start += channels;
        float *s10 = &buf[start];
        start += channels;
        float *s01 = &buf[start];
        start += channels;
        float *s11 = &buf[start];

        for(int m = box->z0; m < box->z1; m++) {
            for(int j = box->y0; j < box->y1; j++) {
                for(int i = box->x0; i < box->x1; i++) {
                    //1D Filtering
                    float mean = FLT_MAX;
                    int indx;
                    float tmpMean;

                    float *tmpDst = (*dst)(i, j, m);

                    //First block
                    BBox tmp00(i - halfKernelSize, i + 1, j - halfKernelSize, j + 1);
                    source->getMeanVal(&tmp00, m00);
                    source->getVarianceVal(m00, &tmp00, s00);

                    tmpMean = 0.0f;

                    for(int l = 0; l < channels; l++) {
                        tmpMean += s00[l];
                    }

                    if(tmpMean < mean) {
                        mean = tmpMean;
                        indx = 0;
                    }

                    //Second block
                    BBox tmp01(i, i + halfKernelSize, j - halfKernelSize, j + 1);
                    source->getMeanVal(&tmp01, m01);
                    source->getVarianceVal(m01, &tmp01, s01);

                    tmpMean = 0.0f;

                    for(int l = 0; l < channels; l++) {
                        tmpMean += s01[l];
                    }

                    if(tmpMean < mean) {
                        mean = tmpMean;
                        indx = 1;
                    }

                    //Third block
                    BBox tmp10(i - halfKernelSize, i + 1, j, j + halfKernelSize);
                    source->getMeanVal(&tmp10, m10);
                    source->getVarianceVal(m10, &tmp10, s10);

                    tmpMean = 0.0f;

                    for(int l = 0; l < channels; l++) {
                        tmpMean += s10[l];
                    }

                    if(tmpMean < mean) {
                        mean = tmpMean;
                        indx = 2;
                    }

                    //Fourth block
                    BBox tmp11(i, i + halfKernelSize, j, j + halfKernelSize);
                    source->getMeanVal(&tmp11, m11);
                    source->getVarianceVal(m11, &tmp11, s11);

                    tmpMean = 0.0f;

                    for(int l = 0; l < channels; l++) {
                        tmpMean += s11[l];
                    }

                    if(tmpMean < mean) {
                        mean = tmpMean;
                        indx = 3;
                    }

                    //final filtering
                    switch(indx) {
                    case 0: {
                        for(int l = 0; l < channels; l++) {
                            tmpDst[l] = m00[l];
                        }
                    }
                    break;

                    case 1: {
                        for(int l = 0; l < channels; l++) {
                            tmpDst[l] = m01[l];
                        }
                    }
                    break;

                    case 2: {
                        for(int l = 0; l < channels; l++) {
                            tmpDst[l] = m10[l];
                        }
                    }
                    break;

                    case 3: {
                        for(int l = 0; l < channels; l++) {
                            tmpDst[l] = m11[l];
                        }
                    }
                    break;

                    default: {
                        float *tmpSrc = (*source)(i, j, m);
                        for(int l = 0; l < channels; l++) {
                            tmpDst[l] = tmpSrc[l];
                        }
                    }break;
                    }
                }
            }
        }

        delete[] buf;
    }

public:
    /**
     * @brief FilterKuwahara
     * @param kernelSize
     */
    FilterKuwahara(int kernelSize = 3)
    {
        update(kernelSize);
    }

    /**
     * @brief update
     * @param kernelSize
     */
    void update(int kernelSize)
    {
        if(kernelSize < 3) {
            kernelSize = 3;
        }

        this->kernelSize = kernelSize;
        halfKernelSize = kernelSize >> 1;
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param kernelSize
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut, int kernelSize)
    {
        FilterKuwahara filter(kernelSize);
        return filter.Process(Single(imgIn), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_KUWAHARA_HPP */

