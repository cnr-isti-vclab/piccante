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

#ifndef PIC_FILTERING_FILTER_LINEAR_COLOR_SPACE_HPP
#define PIC_FILTERING_FILTER_LINEAR_COLOR_SPACE_HPP

#include "filtering/filter.hpp"

namespace pic {

class FilterLinearColorSpace: public Filter
{
protected:
    float		*matrix;
    int			nMatrix;

    //Process in a box
    void ProcessBBox(ImageRAW *dst, ImageRAWVec src, BBox *box)
    {
        if(src[0]->channels != nMatrix) {
            return;
        }

        int width = dst->width;
        int channels = src[0]->channels;
        float *data  = src[0]->data;

        for(int j = box->y0; j < box->y1; j++) {
            int c = j * width;

            for(int i = box->x0; i < box->x1; i++) {
                int c1 = (c + i) * channels;

                for(int k = 0; k < channels; k++) {
                    float sum = 0.0f;
                    int ind   = k * nMatrix;

                    for(int l = 0; l < channels; l++) {
                        sum += data[c1 + l] * matrix[ind + l];
                    }

                    dst->data[c1 + k] = sum;
                }
            }
        }
    }

public:
    //Basic constructor
    FilterLinearColorSpace()
    {
        matrix  = NULL;
        nMatrix = 0;
    }

    ~FilterLinearColorSpace()
    {
        if(matrix != NULL) {
            delete[] matrix;
        }
    }

    float *getRGB2XYZMatrix()
    {
        if(matrix == NULL) {
            matrix = new float[9];
        }

        nMatrix = 3;

        matrix[0] = 0.4124f;
        matrix[1] = 0.3576f;
        matrix[2] = 0.1805f;
        matrix[3] = 0.2126f;
        matrix[4] = 0.7152f;
        matrix[5] = 0.0722f;
        matrix[6] = 0.0193f;
        matrix[7] = 0.1192f;
        matrix[8] = 0.9505f;

        return matrix;
    }

    float *getXYZ2RGBMatrix()
    {
        if(matrix == NULL) {
            matrix = new float[9];
        }

        nMatrix = 3;

        matrix[0] =  3.2406f;
        matrix[1] = -1.5372f;
        matrix[2] = -0.4986f;
        matrix[3] = -0.9689f;
        matrix[4] =  1.8758f;
        matrix[5] =  0.0415f;
        matrix[6] =  0.0557f;
        matrix[7] = -0.2040f;
        matrix[8] =  1.0570f;

        return matrix;
    }

    static ImageRAW *Execute_RGB_to_XYZ(ImageRAW *imgIn, ImageRAW *imgOut)
    {
        FilterLinearColorSpace flt;

        flt.getRGB2XYZMatrix();

        return flt.ProcessP(Single(imgIn), imgOut);
    }

    static ImageRAW *Execute_XYZ_to_RGB(ImageRAW *imgIn, ImageRAW *imgOut)
    {
        FilterLinearColorSpace flt;

        flt.getXYZ2RGBMatrix();

        return flt.ProcessP(Single(imgIn), imgOut);
    }

};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_LINEAR_COLOR_SPACE_HPP */

