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

#ifndef PIC_FILTERING_FILTER_RECONSTRUCT_HPP
#define PIC_FILTERING_FILTER_RECONSTRUCT_HPP

#include "filtering/filter.hpp"

namespace pic {

class FilterReconstruct: public Filter
{
protected:

    //Process in a box
    void ProcessBBox(Image *dst, ImageVec src, BBox *box)
    {
        if(src.size() < 2) {
            return;
        }

        int channels = src[0]->channels;

        for(int j = box->y0; j < box->y1; j++) {

            for(int i = box->x0; i < box->x1; i++) {
                float *tmp_ann = (*src[1])(i, j);
                int x = int(tmp_ann[0]);
                int y = int(tmp_ann[1]);

                float *tmp_dst = (*dst)(i, j);
                float *tmp_src = (*src[0])(x, y);

                for(int k=0;k<channels;k++)
                {
                    tmp_dst[k] = tmp_src[k];
                }
            }
        }
    }

public:
    //Basic constructors
    FilterReconstruct()
    {
    }

    static Image *Execute(Image *imgIn, Image *ann, Image *imgOut = NULL)
    {
        FilterReconstruct fltRec;
        return fltRec.ProcessP(Double(imgIn, ann), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_RECONSTRUCT_HPP */
