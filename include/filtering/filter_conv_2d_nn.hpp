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

#ifndef PIC_FILTERING_FILTER_CONV_2D_NN_HPP
#define PIC_FILTERING_FILTER_CONV_2D_NN_HPP

#include <vector>
#include "filter.hpp"

namespace pic {

class FilterConv2DNN : public Filter
{
protected:

    int inChannels;
    int outChannels;
    int kernelSize;

    std::vector<float> weights;
    std::vector<float> bias;

    void f(FilterFData *data)
    {
        Image *img = data->src[0];

        int x = data->x;
        int y = data->y;
        int z = data->z;

        float *out = data->out;

        const int kernelArea = kernelSize * kernelSize;

        for(int oc = 0; oc < outChannels; oc++) {

            float sum = bias[oc];

            for(int ic = 0; ic < inChannels; ic++) {

                int weightBase = (oc * inChannels + ic) * kernelArea;

                for(int ky = 0; ky < kernelSize; ky++) {

                    const float *src = img->data + z * img->tstride + (y + ky) * img->ystride + x * img->xstride;

                    const int weightRow = weightBase + ky * kernelSize;

                    for(int kx = 0; kx < kernelSize; kx++) {
                        sum += src[kx * img->xstride + ic] * weights[weightRow + kx];
                    }
                }
            }

            out[oc] = sum;
        }
    }

public:

    FilterConv2DNN(int inChannels, int outChannels, int kernelSize)
    {
        this->inChannels  = inChannels;
        this->outChannels = outChannels;
        this->kernelSize  = kernelSize;

        const int nWeights = outChannels * inChannels * kernelSize * kernelSize;

        weights.resize(nWeights, 0.0f);
        bias.resize(outChannels, 0.0f);
    }

    void OutputSize(ImageVec imgIn, int &width, int &height, int &channels, int &frames)
    {
        width = imgIn[0]->width;
        height = imgIn[0]->height;
        channels = outChannels;
        frames   = imgIn[0]->frames;
    }

    void setWeights(const float *w)
    {
        const int n = outChannels * inChannels * kernelSize * kernelSize;

        for(int i = 0; i < n; i++) {
            weights[i] = w[i];
        }
    }

    void setBias(const float *b)
    {
        for(int i = 0; i < outChannels; i++) {
            bias[i] = b[i];
        }
    }

    virtual std::string signature()
    {
        return "CONV2D_NN";
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_CONV_2D_NN_HPP */

