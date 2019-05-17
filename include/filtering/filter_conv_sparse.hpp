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

#ifndef PIC_FILTERING_FILTER_CONV_SPARSE_HPP
#define PIC_FILTERING_FILTER_CONV_SPARSE_HPP

#include "../util/vec.hpp"
#include "../util/array.hpp"
#include "../filtering/filter.hpp"
#include "../util/precomputed_gaussian.hpp"

namespace pic {

struct SparseKernelPoint
{
    Vec<3, int> pos;
    float value;
};

class SparseKernel
{
public:
    std::vector<SparseKernelPoint> data;

    /**
     * @brief SparseKernel
     */
    SparseKernel()
    {

    }

    /**
     * @brief normalize
     */
    void normalize()
    {
        float sum = 0.0f;
        for(int i = 0; i < kernel.size(); i++) {
            sum += kernel[i].value;
        }

        if(sum > 0.0f) {
            for(int i = 0; i < kernel.size(); i++) {
                kernel[i].valu /= sum;
            }
        }
    }
};

/**
 * @brief The FilterConvSparse class
 */
class FilterConvSparse: public Filter
{
protected:

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box);

    SparseKernel kernel;

public:

    /**
     * @brief FilterConvSparse
     */
    FilterConvSparse();

    /**
     * @brief FilterConvSparse
     * @param kernel
     */
    FilterConvSparse(SparseKernel kernel);

    /**
     * @brief update
     * @param kernel
     */
    void update(SparseKernel kernel);

    ~FilterConvSparse();
};

PIC_INLINE FilterConvSparse::FilterConvSparse()
{

}

PIC_INLINE FilterConvSparse::FilterConvSparse(SparseKernel kernel)
{
    update(kernel);
}

PIC_INLINE void FilterConvSparse::update(SparseKernel kernel)
{
    this->kernel = kernel;
}

PIC_INLINE FilterConvSparse::~FilterConvSparse()
{
}

PIC_INLINE void FilterConvSparse::ProcessBBox(Image *dst, ImageVec src, BBox *box)
{
    int channels = dst->channels;

    Image *source = src[0];

    for(int m = box->z0; m < box->z1; m++) {

        for(int j = box->y0; j < box->y1; j++) {

            for(int i = box->x0; i < box->x1; i++) {
                float *dataOut = (*dst)(i, j, m);

                Arrayf::assign(0.0f, dataOut, channels);

                for(int k = 0; k < kernel.data.size(); i++) {
                    float *dataIn = (*source)(i + kernel.data[k].pos[0],
                                              j + kernel.data[k].pos[1],
                                              m + kernel.data[k].pos[2]);

                    for(int ch = 0; ch < channels; ch++) {
                        dataOut[ch] += dataIn[ch] * kernel.data[k].value;
                    }
                }
            }
        }
    }
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_CONV_SPARSE_HPP */

