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

#ifndef PIC_FILTERING_FILTER_CONV_2D_NN_HPP
#define PIC_FILTERING_FILTER_CONV_2D_NN_HPP

#include "../util/array.hpp"

#include "../filtering/filter.hpp"

namespace pic {

class FilterConv2DNN : public Filter
{
protected:

    int inChannels;
    int outChannels;
    int kernelSize;
    int nWeights;
    int kernelArea;


    float *weights;
    float *bias;

    void f(FilterFData *data)
    {
        Image *img = data->src[0];

        int x = data->x;
        int y = data->y;
        int z = data->z;

        int z_stride = z * img->tstride;
        int x_stride = x * img->xstride;
        
        for(int oc = 0; oc < outChannels; oc++) {

            float sum = bias[oc];
            int oc_inChannels = oc * inChannels;

            for(int ic = 0; ic < inChannels; ic++) {

                int weightBase = (oc_inChannels + ic) * kernelArea;

                for(int ky = 0; ky < kernelSize; ky++) {

                    float *src = img->data + z_stride + (y + ky) * img->ystride + x_stride;

                    int weightRow = weightBase + ky * kernelSize;

                    for(int kx = 0; kx < kernelSize; kx++) {
                        sum += src[kx * img->xstride + ic] * weights[weightRow + kx];
                    }
                }
            }

            data->out[oc] = sum;
        }
    }

public:

    FilterConv2DNN(int inChannels, int outChannels, int kernelSize)
    {
        this->inChannels  = inChannels;
        this->outChannels = outChannels;
        this->kernelSize  = kernelSize;
        
        this->kernelArea = kernelSize * kernelSize;


        this->nWeights = outChannels * inChannels * kernelSize * kernelSize;

        weights = new float[nWeights];
        bias = new float[outChannels];
    }

    void OutputSize(ImageVec imgIn, int &width, int &height, int &channels, int &frames)
    {
        width = imgIn[0]->width;
        height = imgIn[0]->height;
        channels = outChannels;
        frames   = imgIn[0]->frames;
    }

    void setWeights(float *weights)
    {
        Arrayf::assign(weights, this->nWeights, this->weights);
    }

    void setBias(float *bias)
    {
        Arrayf::assign(bias, outChannels, this->bias);
    }

    virtual std::string signature()
    {
        return "CONV2D_NN";
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_CONV_2D_NN_HPP */

