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

#ifndef PIC_FILTERING_FILTER_LINEAR_NN_HPP
#define PIC_FILTERING_FILTER_LINEAR_NN_HPP

#include "../util/array.hpp"

#include "../filtering/filter.hpp"

namespace pic {

class FilterLinearNN : public Filter
{
protected:

    int inChannels;
    int outChannels;
    int nWeights;
    int kernelArea;

    float *weights;
    float *bias;

    void f(FilterFData *data)
    {
        Image *img = data->src[0];
        
        float *in = (*img)(data->x, data->y, data->z);
        
        for(int i = 0; i < outChannels; i++) {
            data->out[i] = bias[i];
            data->out[i] += Arrayf::dot(in, &weights[i * inChannels], inChannels);
        }
    }

public:

    FilterLinearNN(int inChannels, int outChannels)
    {
        this->inChannels  = inChannels;
        this->outChannels = outChannels;
        
        this->nWeights = outChannels * inChannels;

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
        return "LINEAR_NN";
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_LINEAR_NN_HPP */

