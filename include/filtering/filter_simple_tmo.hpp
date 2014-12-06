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

#ifndef PIC_FILTERING_FILTER_SIMPLE_TMO_HPP
#define PIC_FILTERING_FILTER_SIMPLE_TMO_HPP

#include "filtering/filter.hpp"

namespace pic {

class FilterSimpleTMO: public Filter
{
protected:
    float gamma, fstop, exposure;

    //Process in a box
    void ProcessBBox(Image *dst, ImageVec src, BBox *box);

public:
    //Basic constructors
    FilterSimpleTMO(float gamma, float fstop);

    //Update the filter
    void Update(float gamma, float fstop);

    static Image *Execute(Image *imgIn, Image *imgOut, float gamma,
                             float fstop)
    {
        FilterSimpleTMO filter(gamma, fstop);
        return filter.ProcessP(Single(imgIn), imgOut);
    }

    static Image *Execute(std::string nameIn, std::string nameOut, float gamma,
                             float fstop)
    {
        Image imgIn(nameIn);
        Image *imgOut = Execute(&imgIn, NULL, gamma, fstop);
        imgOut->Write(nameOut);
        return imgOut;
    }
};

//Basic constructor
FilterSimpleTMO::FilterSimpleTMO(float gamma, float fstop)
{
    Update(gamma, fstop);
}

//Update the filter
void FilterSimpleTMO::Update(float gamma, float fstop)
{
    this->gamma = 1.0f / gamma;
    this->fstop = fstop;
    exposure = powf(2.0f, fstop);
}

//Process in a box
void FilterSimpleTMO::ProcessBBox(Image *dst, ImageVec src, BBox *box)
{

    int width = dst->width;
    int channels = dst->channels;

    float *data = src[0]->data;  

    for(int j = box->y0; j < box->y1; j++) {
        int ind = j * width;

        for(int i = box->x0; i < box->x1; i++) {
            int c = (ind + i) * channels;

            for(int k = 0; k < channels; k++) {
                dst->data[c + k] = powf((data[c + k] * exposure), gamma);
            }
        }
    }
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_SIMPLE_TMO_HPP */

