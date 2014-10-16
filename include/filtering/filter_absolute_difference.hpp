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

#ifndef PIC_FILTERING_FILTER_ABSOLUTE_DIFFERENCE_HPP
#define PIC_FILTERING_FILTER_ABSOLUTE_DIFFERENCE_HPP

#include "filtering/filter.hpp"

namespace pic {

class FilterAbsoluteDifference: public Filter
{
protected:

    // Process in a box
    void ProcessBBox(Image *dst, ImageVec src, BBox *box);

public:

    // Basic constructors
    FilterAbsoluteDifference() {}

    //Execute
    static Image *Execute(Image *imgIn1, Image *imgIn2)
    {
        FilterAbsoluteDifference filter;
        return filter.Process(Double(imgIn1, imgIn2), NULL);
    }

    //Execute
    static Image *Execute(std::string name1, std::string name2,
                             std::string nameOut)
    {
        Image imgIn1(name1);
        Image imgIn2(name2);

        Image *out = Execute(&imgIn1, &imgIn2);
        out->Write(nameOut);
        return out;
    }
};

// Process in a box
void FilterAbsoluteDifference::ProcessBBox(Image *dst, ImageVec src,
        BBox *box)
{
    if(src.size()!=2) {
        return;
    }

    int channels = dst->channels;

    for(int i = box->y0; i < box->y1; i++) {
       for(int j = box->x0; j < box->x1; j++) {

            float *tmp_src0 = (*src[0])(j, i);
            float *tmp_src1 = (*src[1])(j, i);
            float *tmp_dst  = (*dst   )(j, i);

            for(int k = 0; k < channels; k++) {
                tmp_dst[k] = fabsf(tmp_src0[k] - tmp_src1[k]);
            }
        }
    }
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_ABSOLUTE_DIFFERENCE_HPP */

