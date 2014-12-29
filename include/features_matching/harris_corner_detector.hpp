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

#ifndef PIC_FEATURES_MATCHING_HARRIS_CORNER_DETECTOR_HPP
#define PIC_FEATURES_MATCHING_HARRIS_CORNER_DETECTOR_HPP

#include "util/vec.hpp"

#include "image.hpp"
#include "filtering/filter_luminance.hpp"
#include "filtering/filter_gaussian_2d.hpp"
#include "filtering/filter_conv_1d.hpp"
#include "filtering/filter_max.hpp"
#include "features_matching/general_corner_detector.hpp"

#ifndef PIC_DISABLE_EIGEN
#include "externals/Eigen/Dense"
#endif

namespace pic {

#ifndef PIC_DISABLE_EIGEN

/**
 * @brief The HarrisCornerDetector class
 */
class HarrisCornerDetector: public GeneralCornerDetector
{
protected:
    Image *Ix, *Iy, *Ixy, *ret;
    Image *Ix2_flt, *Iy2_flt, *Ixy_flt;
    Image *filtered;

    //Harris Corners detector parameters
    float sigma, threshold;
    int   radius;

    //previous values
    int width, height;

    /**
     * @brief Destroy
     */
    void Destroy()
    {
        if(lum != NULL) {
            delete lum;
        }

        lum = NULL;

        if(Ix != NULL) {
            delete Ix;
        }

        Ix = NULL;

        if(Iy != NULL) {
            delete Iy;
        }

        Iy = NULL;

        if(Ix2_flt != NULL) {
            delete Ix2_flt;
        }

        Ix2_flt = NULL;

        if(Iy2_flt != NULL) {
            delete Iy2_flt;
        }

        Iy2_flt = NULL;

        if(Ixy_flt != NULL) {
            delete Ixy_flt;
        }

        Ixy_flt = NULL;

        if(filtered != NULL) {
            delete filtered;
        }

        filtered = NULL;

        if(ret != NULL) {
            delete ret;
        }

        ret = NULL;
    }

    /**
     * @brief SetNULL
     */
    void SetNULL()
    {
        width = -1;
        height = -1;
        lum = NULL;
        Ix = NULL;
        Iy = NULL;
        Ixy = NULL;
        Ix2_flt = NULL;
        Iy2_flt = NULL;
        Ixy_flt = NULL;
        filtered = NULL;
        ret = NULL;
    }

public:

    /**
     * @brief HarrisCornerDetector
     * @param sigma
     * @param radius
     * @param threshold
     */
    HarrisCornerDetector(float sigma = 1.0f, int radius = 3, float threshold = 0.001f) : GeneralCornerDetector()
    {
        SetNULL();
        Update(sigma, radius, threshold);
    }

    ~HarrisCornerDetector()
    {
        Destroy();
    }

    /**
     * @brief Update
     * @param sigma
     * @param radius
     * @param threshold
     */
    void Update(float sigma = 1.0f, int radius = 3, float threshold = 0.001f)
    {
        if(sigma > 0.0f) {
            this->sigma = sigma;
        } else {
            this->sigma = 1.0f;
        }

        if(radius > 0) {
            this->radius = radius;
        } else {
            this->radius = 1;
        }

        this->threshold = threshold;
    }

    /**
     * @brief Compute
     * @param img
     * @param corners
     */
    void Compute(Image *img, std::vector< Eigen::Vector3f > *corners)
    {
        if(img == NULL) {
            return;
        }

        if((img->width != width) || (img->height != height)) {
            width = img->width;
            height = img->height;

            Destroy();
        }

        if(img->channels == 1) {
            lum = img->Clone();
        } else {
            lum = FilterLuminance::Execute(img, lum, LT_CIE_LUMINANCE);
        }

        float maxL = lum->getMaxVal(NULL, NULL)[0];
        float minL = lum->getMinVal(NULL, NULL)[0];

        float delta = maxL - minL;

        *lum -= minL;
        *lum *= delta;

        if(corners == NULL) {
            corners = new std::vector< Eigen::Vector3f >;
        }

        corners->clear();

        float kernel[] = { -1.0f, 0.0f, 1.0f};

        //Gradients
        Ix = FilterConv1D::Execute(lum, Ix, kernel, 3, true);
        Iy = FilterConv1D::Execute(lum, Iy, kernel, 3, false);

        if(Ixy == NULL) {
            Ixy = Ix->Clone();
        } else {
            *Ixy = *Ix;
        }

        *Ixy *= *Iy;

        *Ix *= *Ix;
        *Iy *= *Iy;

        float eps = 2.2204e-16f;

        //Filtering the values
        FilterGaussian2D flt(sigma);
        Ix2_flt = flt.ProcessP(Single(Ix), Ix2_flt);
        Iy2_flt = flt.ProcessP(Single(Iy), Iy2_flt);
        Ixy_flt = flt.ProcessP(Single(Ixy), Ixy_flt);

        //ret = (Ix2.*Iy2 - Ixy.^2)./(Ix2 + Iy2 + eps);
        *Ixy_flt *= *Ixy_flt; //Ixy.^2

        if(ret == NULL) {
            ret = Ix2_flt->Clone();
        } else {
            *ret = *Ix2_flt;
        }

        *ret *= *Iy2_flt; //Ix2.*Iy2
        *ret -= *Ixy_flt;

        *Ix2_flt += *Iy2_flt;
        *Ix2_flt += eps;

        *ret /= *Ix2_flt;

        //Maximal supression
        filtered = FilterMax::Execute(ret, filtered, radius * 2 + 1);

        float w = 1.0f;

        if(threshold < 0.0f) { //the best i-th points
            int bestPoints = int(-threshold);

            ret->sort();

            int n = ret->size();
            threshold = ret->dataTMP[n - 1 - bestPoints];
        }

        int width = lum->width;
        int height = lum->height;
        float *data = ret->data;

        for(int i = 1; i < (height - 1); i++) {
            int tmp = i * width;

            float i_f = float(i);
            float cx, cy, ax, ay, bx, by, x, y;

            for(int j = 1; j < (width - 1); j++) {
                int ind = tmp + j;

                if((data[ind] == filtered->data[ind]) && (data[ind] > threshold)) {

                    cx = data[ind];
                    ax = (data[ind - 1] + data[ind + 1]) / 2.0f - cx;
                    bx = ax + cx - data[ind - 1];
                    x = -w * bx / (2.0f * ax);

                    cy = data[ind];
                    ay = (data[ind - width] + data[ind + width]) / 2.0f - cy;
                    by = ay + cy - data[ind - width];
                    y = -w * by / (2.0f * ay);

                    corners->push_back(Eigen::Vector3f(float(j) + x, i_f + y, data[ind]));
                }
            }
        }
    }
};

#endif

} // end namespace pic

#endif /* PIC_FEATURES_MATCHING_HARRIS_CORNER_DETECTOR_HPP */

