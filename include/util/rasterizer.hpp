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

#ifndef PIC_UTIL_RASTERIZER_HPP
#define PIC_UTIL_RASTERIZER_HPP

#include <algorithm>
#include <cstdlib>

#include "../base.hpp"
#include "../image.hpp"
#include "../util/vec.hpp"
#include "../util/math.hpp"

#ifndef PIC_EIGEN_NOT_BUNDLED
    #include "../externals/Eigen/Dense"
#else
    #include <Eigen/Dense>
#endif

namespace pic {

/**
 * @brief drawLine renders a line (v0, v1) with color into img.
 * @param img is the image where to render the line (v0, v1).
 * @param v0 is the first vertex of the line.
 * @param v1 is the second vertex of the line.
 * @param color is the color of the line (v0, v1).
 */
PIC_INLINE void drawLine(Image *img, Vec2i v0, Vec2i v1, float *color)
{
    if(img == NULL || color == NULL) {
        return;
    }

    CLAMP(v0[0], img->width);
    CLAMP(v1[0], img->width);
    CLAMP(v0[1], img->height);
    CLAMP(v1[1], img->height);

    float *data = img->data;

    if(v0[0] > v1[0]) {
        std::swap(v0, v1);
    }

    int dx = v1[0] - v0[0];
    int dy = v1[1] - v0[1];

    //Vertical line
    if(dx == 0){
        if(v0[1] > v1[1]) {
            std::swap(v0, v1);
        }

        for(int y = v0[1]; y < v1[1]; y++) {
            int ind = (v0[0] + y * img->width) * img->channels;

            for(int k = 0; k < img->channels; k++) {
                data[ind + k] = color[k];
            }
        }
        return;
    }

    //Horizontal line
    if(dy == 0) {
        int ind_y = v0[1] * img->width;

        for(int x = v0[0]; x < v1[0]; x++) {
            int ind = (x + ind_y) * img->channels;

            for(int k = 0; k < img->channels; k++) {
                data[ind + k] = color[k];
            }
        }
    }

    //General case
    if(std::abs(dy) < std::abs(dx)) {
        //m < 1
        int e = 0;
        int s;
        if(dy < 0) {
            s  = -1;
            dy = -dy;
        } else {
            s = 1;
        }

        int y = v0[1];
        for(int x = v0[0]; x <= v1[0]; x++) {
            int ind = (x + y * img->width) * img->channels;

            for(int k = 0; k < img->channels; k++) {
                data[ind + k] = color[k];
            }

            e += dy;
            if((e << 1) >= dx) {
              y += s;
              e -= dx;
            }
        }
    } else {
        //m > 1
        if(v0[1] > v1[1]){
            std::swap(v0, v1);
        }

        dx = v1[0] - v0[0];
        dy = v1[1] - v0[1];

        int e = 0;
        int s;
        if(dx < 0){
            s  = -1;
            dx = -dx;
        } else {
            s = 1;
        }

        int x = v0[0];

        for(int y = v0[1]; y <= v1[1]; y++) {

            int ind = (x + y * img->width) * img->channels;

            for(int k = 0; k < img->channels; k++) {
                data[ind + k] = color[k];
            }

            e += dx;
            if((e << 1) >= dy) {
              x += s;
              e -= dy;
            }
        }
    }
}

/**
 * @brief drawPoints
 * @param img
 * @param points
 */
#ifndef PIC_DISABLE_EIGEN
PIC_INLINE void drawPoints(Image *img, std::vector< Eigen::Vector2f > &points, float *color)
{
    if(img == NULL) {
        return;
    }

    if(color == NULL) {
        color = new float[img->channels];
        for(int i = 0; i < img->channels; i++) {
            color[i] = 1.0f;
        }
    }

    for(size_t i = 0; i < points.size(); i++) {
        int x = int(points[i][0]);
        int y = int(points[i][1]);
        float *pixel = (*img)(x, y);

        for(int j = 0; j < img->channels; j++) {
            pixel[j] = color[j];
        }
    }
}
#endif

/**
 * @brief evaluateGaussian renders a Gaussian function which is centred
 * in the image.
 * @param img is an input image
 * @param sigma is the standard deviation of the Gaussian function.
 * @param bNormTerm is a boolean value. If it is true the Gaussian function
 * is normalized, false otherwise.
 */
PIC_INLINE void evaluateGaussian(Image *img, float sigma = -1.0f,
                                 bool bNormTerm = false)
{
    if(img != NULL) {
        return;
    }

    if(sigma < 0.0f) {
        sigma = float(MIN(img->width, img->height)) / 5.0f;
    }

    float sigma2 = (sigma * sigma * 2.0f);

    int halfWidth  = img->width  >> 1;
    int halfHeight = img->height >> 1;

    float normTerm = bNormTerm ? sigma * sqrtf(C_PI) : 1.0f ;

    #pragma omp parallel for

    for(int j = 0; j < img->height; j++) {
        int j_squared = j - halfHeight;
        j_squared = j_squared * j_squared;

        for(int i = 0; i < img->width; i++) {
            int i_squared = i - halfWidth;
            i_squared = i_squared * i_squared;

            float gaussVal = expf(-float(i_squared + j_squared) / sigma2) / normTerm;

            float *tmp_data = (*img)(i, j);

            for(int k = 0; k < img->channels; k++) {
                tmp_data[k] = gaussVal;
            }
        }
    }
}

/**
 * @brief evaluateSolid renders a centred circle.
 * @param img is an input image
 */
PIC_INLINE void evaluateSolid(Image *img)
{
    if(img == NULL) {
        return;
    }

    int halfWidth  = img->width  >> 1;
    int halfHeight = img->height >> 1;

    int radius_squared = (halfWidth * halfWidth + halfHeight * halfHeight) >> 1;

    #pragma omp parallel for

    for(int j = 0; j < img->height; j++) {
        int j_squared = j - halfHeight;
        j_squared = j_squared * j_squared;

        for(int i = 0; i < img->width; i++) {
            int i_squared = i - halfWidth;
            i_squared = i_squared * i_squared;

            float val = 0.0f;

            if((i_squared + j_squared) < radius_squared) {
                val = 1.0f;
            }

            float *tmp_data = (*img)(i, j);

            for(int k = 0; k < img->channels; k++) {
                tmp_data[k] = val;
            }
        }
    }
}

}

#endif //
