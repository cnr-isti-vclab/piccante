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

#ifndef PIC_FILTERING_FILTER_CONV_1D_HPP
#define PIC_FILTERING_FILTER_CONV_1D_HPP

#include "../base.hpp"
#include "../util/std_util.hpp"
#include "../util/array.hpp"
#include "../filtering/filter.hpp"
#include "../util/precomputed_gaussian.hpp"

namespace pic {

/**
 * @brief The FilterConv1D class
 */
class FilterConv1D: public Filter
{
protected:
    int dirs[3];
    float *data; //NOTE: this an external pointer; NEVER release it!
    int kernelSize, halfKernelSize;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box);

public:

    /**
     * @brief FilterConv1D
     */
    FilterConv1D();

    /**
     * @brief FilterConv1D
     * @param data
     * @param n
     * @param direction
     */
    FilterConv1D(float *data, int kernelSize, int direction);

    ~FilterConv1D();

    /**
     * @brief update
     * @param data
     * @param n
     * @param direction
     */
    void update(float *data, int kernelSize, int direction);

    /**
     * @brief changePass
     * @param pass
     * @param tPass
     */
    void changePass(int pass, int tPass);

    /**
     * @brief ChangePass
     * @param x
     * @param y
     * @param z
     */
    void changePass(int x, int y, int z);

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param data
     * @param n
     * @param XorY
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut, float *data, int n,
                             bool XorY = true)
    {
        FilterConv1D filter(data, n, 0);

        if(XorY) {
            filter.changePass(1, 0, 0);
        } else {
            filter.changePass(0, 1, 0);
        }

        return filter.Process(Single(imgIn), imgOut);
    }

    /**
     * @brief getKernelMean creates an 1D mean kernel.
     * @param kernelSize
     * @return
     */
    static float *getKernelMean(int kernelSize)
    {
        if(kernelSize < 3) {
            kernelSize = 3;
        }
        
        if((kernelSize % 2) == 0) {
            kernelSize++;
        }
        
        float *kernel = new float[kernelSize];

        float val = 1.0f / float(kernelSize);

        for(int i = 0; i < kernelSize; i++) {
            kernel[i] = val;
        }

        return kernel;
    }
};

PIC_INLINE FilterConv1D::FilterConv1D()
{
    kernelSize = 0;
    halfKernelSize = 0;
    data = NULL;

    dirs[0] = 0;
    dirs[1] = 0;
    dirs[2] = 0;
}

PIC_INLINE FilterConv1D::FilterConv1D(float *data, int kernelSize, int direction = 0)
{
    update(data, kernelSize, direction);
}

PIC_INLINE FilterConv1D::~FilterConv1D()
{
    data = NULL;
    kernelSize = -1;
    halfKernelSize = -1;
}

PIC_INLINE void FilterConv1D::update(float *data, int kernelSize, int direction)
{
    if(data == NULL || kernelSize < 1) {
        return;
    }

    this->data = data;
    this->kernelSize = kernelSize;

    this->halfKernelSize = kernelSize >> 1;

    if(direction > 0) {
        dirs[ direction      % 3] = 1;
        dirs[(direction + 1) % 3] = 0;
        dirs[(direction + 2) % 3] = 0;
    }
}

PIC_INLINE void FilterConv1D::changePass(int pass, int tPass)
{
    int tMod;

    if(tPass > 1) {
        tMod = 3;
    } else {
        if(tPass == 1) {
            tMod = 2;
        } else {
            printf("ERROR: FilterConv1D::ChangePass");
            return;
        }
    }

    dirs[pass % tMod] = 1;

    for(int i = 1; i < tMod; i++) {
        dirs[(pass + i) % tMod] = 0;
    }
    
    #ifdef PIC_DEBUG
        printf("%d %d %d\n",dirs[0],dirs[1],dirs[2]);
    #endif
}

PIC_INLINE void FilterConv1D::changePass(int x, int y, int z)
{
    dirs[0] = y;
    dirs[1] = x;
    dirs[2] = z;
}

PIC_INLINE void FilterConv1D::ProcessBBox(Image *dst, ImageVec src, BBox *box)
{
    Image *source = src[0];

    for(int m = box->z0; m < box->z1; m++) {

        for(int j = box->y0; j < box->y1; j++) {

            for(int i = box->x0; i < box->x1; i++) {
                float *dst_data = (*dst)(i, j, m);

                Arrayf::assign(0.0f, dst_data, dst->channels);

                for(int k = 0; k < kernelSize; k++) { //1D Filtering
                    int tmpCoord = k - halfKernelSize;

                    //Address cj
                    int cj = j + tmpCoord * dirs[0];
                    //Address ci
                    int ci = i + tmpCoord * dirs[1];
                    //Address cm
                    int cm = m + tmpCoord * dirs[2];

                    float *tmpSource = (*source)(ci, cj, cm);

                    for(int l = 0; l < dst->channels; l++) {
                        dst_data[l] += tmpSource[l] * data[k];
                    }
                }
            }
        }
    }
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_CONV_1D_HPP */

