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

#ifndef PIC_FILTERING_FILTER_DIVERGENCE_HPP
#define PIC_FILTERING_FILTER_DIVERGENCE_HPP

#include "filtering/filter.hpp"

namespace pic {

class FilterDivergence: public Filter
{
protected:
    int colorChannel;

    //Process in a box
    void ProcessBBox(Image *dst, ImageVec src, BBox *box);

public:
    //Basic constructors
    FilterDivergence() {}

    static Image *Execute(Image *imgIn, Image *imgOut)
    {
        FilterDivergence filter;
        return filter.ProcessP(Single(imgIn), imgOut);
    }

    static Image *Execute(std::string nameIn, std::string nameOut)
    {
        Image imgIn(nameIn);
        Image *imgOut = Execute(&imgIn, NULL);
        imgOut->Write(nameOut);
        return imgOut;
    }
};

//Process in a box
void FilterDivergence::ProcessBBox(Image *dst, ImageVec src, BBox *box)
{

    int width = dst->width;
    int height = dst->height;

    //Filtering
    float *data = src[0]->data;
    float gradX, gradY;

    int channels = src[0]->channels;
    int c, ci, cj, ci1, cj1, tmpc, ind;

    for(int j = box->y0; j < box->y1; j++) {
        ind = j * width;

        for(int i = box->x0; i < box->x1; i++) {
            c = (ind + i) * channels;
            //Positions
            ci  = CLAMP(i + 1, width);
            cj  = CLAMP(j + 1, height);
            ci1 = CLAMP(i - 1, width);
            cj1 = CLAMP(j - 1, height);

            //Grad X
            tmpc  = (ind + ci) * channels;
            gradX = data[tmpc];

            tmpc  = (ind + ci1) * channels;
            gradX -= data[tmpc];

            //Grad Y
            tmpc  = (cj * width + i) * channels;
            gradY = data[tmpc];

            tmpc  = (cj1 * width + i) * channels;
            gradY -= data[tmpc];

            //Divergence
            dst->data[c] = (gradX + gradY) * 0.5f;
        }
    }
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_DIVERGENCE_HPP */

