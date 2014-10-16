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

#ifndef PIC_FILTERING_FILTER_LAPLACIAN_HPP
#define PIC_FILTERING_FILTER_LAPLACIAN_HPP

#include "filtering/filter.hpp"

namespace pic {

class FilterLaplacian: public Filter
{
protected:

    //Process in a box
    void ProcessBBox(Image *dst, ImageVec src, BBox *box)
    {
        int width = dst->width;
        int height = dst->height;
        int channels = src[0]->channels;
        float *data = src[0]->data;

        float sum;
        int tmp[4], cip1, cjp1, cim1, cjm1;

        for(int j = box->y0; j < box->y1; j++) {
            int ind = j * width;

            for(int i = box->x0; i < box->x1; i++) {
                int c = (ind + i) * channels;

                //neighbors
                tmp[0]  = CLAMP(i + 1, width);
                cip1    = (ind + tmp[0]) * channels;

                tmp[1]  = CLAMP(i - 1, width);
                cim1    = (ind + tmp[1]) * channels;

                tmp[2]  = CLAMP(j + 1, height);
                cjp1    = (tmp[2] * width + i) * channels;

                tmp[3]  = CLAMP(j - 1, height);
                cjm1    = (tmp[3] * width + i) * channels;

                for(int k = 0; k < channels; k++) {
                    sum  = -4.0f * data[c + k];
                    sum += data[cip1 + k];
                    sum += data[cim1 + k];
                    sum += data[cjp1 + k];
                    sum += data[cjm1 + k];

                    dst->data[c + k] = sum;
                }
            }
        }
    }

public:
    //Basic constructor
    FilterLaplacian() {}

    static Image *Execute(Image *imgIn, Image *imgOut)
    {
        FilterLaplacian filter;
        return filter.ProcessP(Single(imgIn), imgOut);
    }

    static void Execute(std::string nameIn, std::string nameOut)
    {
        Image imgIn(nameIn);
        Image *imgOut = Execute(&imgIn, NULL);
        imgOut->Write(nameOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_LAPLACIAN_HPP */

