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

#include "../util/vec.hpp"

#include "../image.hpp"
#include "../filtering/filter_luminance.hpp"
#include "../filtering/filter_gaussian_2d.hpp"
#include "../filtering/filter_gradient_harris_opt.hpp"
#include "../filtering/filter_max.hpp"
#include "../features_matching/general_corner_detector.hpp"

#ifndef PIC_DISABLE_EIGEN

#ifndef PIC_EIGEN_NOT_BUNDLED
    #include "../externals/Eigen/Dense"
#else
    #include <Eigen/Dense>
#endif

#endif

namespace pic {

#ifndef PIC_DISABLE_EIGEN

/**
 * @brief The HarrisCornerDetector class
 */
class HarrisCornerDetector: public GeneralCornerDetector
{
protected:
    Image *I_grad;
    Image *I_grad_flt;
    Image *ret;

    //Harris Corners detector parameters
    float sigma, threshold;
    int   radius;

    //previous values
    int width, height;

    /**
     * @brief release
     */
    void release()
    {
        if(lum != NULL) {
            delete lum;
        }

        lum = NULL;

        if(I_grad != NULL) {
            delete I_grad;
        }

        I_grad = NULL;

        if(I_grad_flt != NULL) {
            delete I_grad_flt;
        }

        I_grad_flt = NULL;

        if(ret != NULL) {
            delete ret;
        }

        ret = NULL;
    }

    /**
     * @brief setNULL
     */
    void setNULL()
    {
        width = -1;
        height = -1;
        lum = NULL;
        I_grad = NULL;
        I_grad_flt = NULL;
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
        setNULL();
        update(sigma, radius, threshold);
    }

    ~HarrisCornerDetector()
    {
        release();
    }

    /**
     * @brief update
     * @param sigma
     * @param radius
     * @param threshold
     */
    void update(float sigma = 1.0f, int radius = 3, float threshold = 0.001f)
    {
        this->sigma = sigma > 0.0f ? sigma : 1.0f;
        this->radius = radius > 0 ? radius : 1;
        this->threshold = threshold > 0.0f ? threshold : 0.001f;
    }

    /**
     * @brief execute
     * @param img
     * @param corners
     */
    void execute(Image *img, std::vector< Eigen::Vector2f > *corners)
    {
        if(img == NULL || corners == NULL) {
            return;
        }

        if((img->width != width) || (img->height != height)) {
            width = img->width;
            height = img->height;

            release();
        }

        if(img->channels == 1) {
            lum = img->clone();
        } else {
            lum = FilterLuminance::execute(img, lum, LT_CIE_LUMINANCE);
        }

        float minL, maxL;
        lum->getMinVal(NULL, &minL);
        lum->getMaxVal(NULL, &maxL);

        float delta = maxL - minL;

        *lum -= minL;
        *lum *= delta;

        corners->clear();

        std::vector< Eigen::Vector3f > corners_w_quality;

        //compute gradients
        I_grad = FilterGradientHarrisOPT::execute(lum, I_grad, 0);

        float eps = 2.2204e-16f;

        //filter gradient values
        FilterGaussian2D flt(sigma);
        I_grad_flt = flt.Process(Single(I_grad), I_grad_flt);

        if(ret == NULL) {
            ret = lum->allocateSimilarOne();
        }

        //ret = (Ix2.*Iy2 - Ixy.^2)./(Ix2 + Iy2 + eps);
        for(int i = 0; i < height; i++) {
            for(int j = 0; j < width; j++) {
                float *data_ret = (*ret)(j, i);

                float *I_grad_val = (*I_grad_flt)(j, i);

                float x2 = I_grad_val[0];
                float y2 = I_grad_val[1];
                float xy = I_grad_val[2];

                data_ret[0] =  (x2 * y2 - xy * xy) / (x2 + y2 + eps);
            }
        }

        //non-maximal supression
        lum = FilterMax::execute(ret, lum, radius * 2 + 1);
        Image* ret_flt = lum;

        float w = 1.0f;

        if(threshold < 0.0f) { //the best i-th points
            int bestPoints = int(-threshold);

            ret->sort();

            int n = ret->size();
            threshold = ret->dataTMP[n - 1 - bestPoints];
        }

        for(int i = 0; i < height; i++) {
            float i_f = float(i);
            float cx, cy, ax, ay, bx, by, x, y;

            for(int j = 0; j < width; j++) {

                float R = (*ret)(j, i)[0];
                float R_flt = (*ret_flt)(j, i)[0];

                if((R == R_flt) && (R > threshold)) {
                    float Rr = (*ret)(j, i + 1)[0];
                    float Rl = (*ret)(j, i - 1)[0];
                    float Ru = (*ret)(j + 1, i)[0];
                    float Rd = (*ret)(j - 1, i)[0];

                    cx = R;
                    ax = (Rl + Rr) / 2.0f - cx;
                    bx = ax + cx - Rl;

                    if(ax != 0.0f) {
                        x = -w * bx / (2.0f * ax);
                    } else {
                        x = 0.0f;
                    }

                    cy = R;
                    ay = (Rd + Ru) / 2.0f - cy;
                    by = ay + cy - Rd;

                    if(ay != 0.0f) {
                        y = -w * by / (2.0f * ay);
                    } else {
                        y = 0.0f;
                    }

                    corners_w_quality.push_back(Eigen::Vector3f(float(j) + x, i_f + y, R));
                }
            }
        }

        sortCornersAndTransfer(&corners_w_quality, corners);
    }
};

#endif

} // end namespace pic

#endif /* PIC_FEATURES_MATCHING_HARRIS_CORNER_DETECTOR_HPP */

