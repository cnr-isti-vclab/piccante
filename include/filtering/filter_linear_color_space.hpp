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

#ifndef PIC_FILTERING_FILTER_LINEAR_COLOR_SPACE_HPP
#define PIC_FILTERING_FILTER_LINEAR_COLOR_SPACE_HPP

#include "../filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterLinearColorSpace class
 */
class FilterLinearColorSpace: public Filter
{
protected:
    float		*matrix;
    int			nMatrix;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box)
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
    /**
     * @brief FilterLinearColorSpace
     */
    FilterLinearColorSpace() : Filter()
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

    /**
     * @brief getRGB2XYZMatrix
     * @return
     */
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

    /**
     * @brief getXYZ2RGBMatrix
     * @return
     */
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

    /**
     * @brief execute_RGB_to_XYZ
     * @param imgIn
     * @param imgOut
     * @return
     */
    static Image *execute_RGB_to_XYZ(Image *imgIn, Image *imgOut)
    {
        FilterLinearColorSpace flt;

        flt.getRGB2XYZMatrix();

        return flt.Process(Single(imgIn), imgOut);
    }

    /**
     * @brief execute_XYZ_to_RGB
     * @param imgIn
     * @param imgOut
     * @return
     */
    static Image *execute_XYZ_to_RGB(Image *imgIn, Image *imgOut)
    {
        FilterLinearColorSpace flt;

        flt.getXYZ2RGBMatrix();

        return flt.Process(Single(imgIn), imgOut);
    }

};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_LINEAR_COLOR_SPACE_HPP */

