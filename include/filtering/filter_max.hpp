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

#ifndef PIC_FILTERING_FILTER_MAX_HPP
#define PIC_FILTERING_FILTER_MAX_HPP

#include "filtering/filter.hpp"

namespace pic {

class FilterMax: public Filter
{
protected:
    int halfSize;

    //Process in a box
    void ProcessBBox(ImageRAW *dst, ImageRAWVec src, BBox *box)
    {
        int width = dst->width;
        int channels = dst->channels;

        float *data = src[0]->data;
        
        for(int j = box->y0; j < box->y1; j++) {
            int ind = j * width;

            for(int i = box->x0; i < box->x1; i++) {
                //Central weight
                int c = (ind + i) * channels;

                for(int ch = 0; ch < channels; ch++) {
                    float maxVal = -FLT_MAX;

                    for(int k = -halfSize; k <= halfSize; k++) {
                        int ci = CLAMP((i + k), dst->width);

                        for(int l = -halfSize; l <= halfSize; l++) {
                            int cj = CLAMP((j + l), dst->height);
                            float tmp = data[(cj * width + ci) * channels + ch];
                            maxVal = maxVal > tmp ? maxVal : tmp;
                        }
                    }

                    dst->data[c + ch] = maxVal;
                }
            }
        }
    }

public:
    //Basic constructor
    FilterMax(int size)
    {
        this->halfSize = checkHalfSize(size);
    }

    static ImageRAW *Execute(ImageRAW *imgIn, ImageRAW *imgOut, int size)
    {
        FilterMax filter(size);
        return filter.ProcessP(Single(imgIn), imgOut);
    }

    static ImageRAW *Execute(std::string nameIn, std::string nameOut, int size)
    {
        ImageRAW imgIn(nameIn);
        ImageRAW *imgOut = Execute(&imgIn, NULL, size);
        imgOut->Write(nameOut);
        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_MAX_HPP */

