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

#ifndef PIC_FEATURES_MATCHING_CANNY_EDGE_DETECTOR_HPP
#define PIC_FEATURES_MATCHING_CANNY_EDGE_DETECTOR_HPP

#include "../util/vec.hpp"
#include "../util/std_util.hpp"
#include "../image.hpp"

#include "../filtering/filter_luminance.hpp"
#include "../filtering/filter_gaussian_2d.hpp"
#include "../filtering/filter_gradient.hpp"

namespace pic {

/**
 * @brief The CannyEdgeDetector class
 */
class CannyEdgeDetector
{
protected:
    bool  bLum;
    Image *lum;

    float sigma, threshold_1, threshold_2;

    /**
     * @brief release frees allocated memory for this class.
     */
    void release()
    {
        if(bLum) {
            lum = delete_s(lum);
        }

        bLum = false;
    }

public:
    /**
     * @brief CannyEdgeDetector
     */
    CannyEdgeDetector()
    {
        lum = NULL;
        bLum = false;

        update();
    }

    ~CannyEdgeDetector()
    {
        release();
    }

    /**
     * @brief update
     * @param sigma
     * @param threshold_1
     * @param threshold_2
     */
    void update(float sigma = 1.4f, float threshold_1 = 0.05f, float threshold_2 = 0.3f)
    {
        this->sigma = sigma > 0.0f ? sigma : 1.4f;
        this->threshold_1 = threshold_1 > 0.0f ? threshold_1 : 0.05f;
        this->threshold_2 = threshold_2 > 0.0f ? threshold_1 : 0.3f;

        if(threshold_2 < threshold_1) {
            float tmp = threshold_1;
            threshold_1 = threshold_2;
            threshold_2 = tmp;
        }
    }

    /**
     * @brief execute executes Canny edge detector on img and ouputs imgEdges as results.
     * @param img
     * @param imgEdges
     * @return
     */
    Image *execute(Image *img, Image *imgOut)
    {
        if(img == NULL) {
            return imgOut;
        }

        if(img->channels == 1) {
            release();
            lum = img;
        } else {
            bLum = true;
            lum = FilterLuminance::execute(img, lum, LT_CIE_LUMINANCE);
        }

        //filter the image
        FilterGaussian2D flt(sigma);
        Image *lum_flt = flt.Process(Single(lum), NULL);

        FilterGradient fltGrad(0, G_SOBEL);
        Image *grad = fltGrad.Process(Single(lum_flt), NULL);

        //non-maximum suppression
        if(imgOut == NULL) {
            imgOut = lum->allocateSimilarOne();
        }

        imgOut->setZero();

        int dx0[] = {1, 1, 0, -1, 1};
        int dy0[] = {0, 1, 1,  1, 0};

        int dx1[] = {-1, -1,  0,  1, -1};
        int dy1[] = { 0, -1, -1, -1,  0};

        for(int i = 0; i < grad->height; i++) {
            for(int j = 0; j < grad->width; j++) {

                float *tmp_grad = (*grad)(j, i);

                float angle = atan2(tmp_grad[1], tmp_grad[0]);

                angle = Rad2Deg(angle < 0.0f ? C_PI + angle : angle);
                int k = int(lround(angle / 45.0f));

                bool bMax = (tmp_grad[2] > (*grad)(j + dx0[k], i + dy0[k])[2]) &&
                            (tmp_grad[2] > (*grad)(j + dx1[k], i + dy1[k])[2]);

                /*
                bool bMax = false;

                if(((angle >=   0.0f) && (angle < 22.5f)) ||
                   ((angle >= 157.5f))) {
                    bMax = (tmp_grad[2] > (*grad)(j + 1, i)[2]) &&
                           (tmp_grad[2] > (*grad)(j - 1, i)[2]);
                }

                if((angle >= 22.5f) && (angle < 67.5f)) {
                    bMax = (tmp_grad[2] > (*grad)(j + 1, i + 1)[2]) &&
                           (tmp_grad[2] > (*grad)(j - 1, i - 1)[2]);
                }

                if((angle >= 67.5f) && (angle < 112.5f)) {
                    bMax = (tmp_grad[2] > (*grad)(j, i + 1)[2]) &&
                           (tmp_grad[2] > (*grad)(j, i - 1)[2]);
                }

                if((angle >= 112.5f) && (angle < 157.5f)) {
                    bMax = (tmp_grad[2] > (*grad)(j - 1, i + 1)[2]) &&
                           (tmp_grad[2] > (*grad)(j + 1, i - 1)[2]);
                }*/

                float *imgOut_ji = (*imgOut)(j, i);

                imgOut_ji[0] = bMax ? tmp_grad[2] : 0.0f;
            }
        }

        //double thresholding
        for(int i = 0; i < imgOut->height; i++) {
            for(int j = 0; j < imgOut->width; j++) {
                float *imgOut_ji = (*imgOut)(j, i);

                if(imgOut_ji[0] > threshold_2) {
                    imgOut_ji[0] = 1.0f; //strong edge
                } else {
                    //0.5f --> weak edge
                    //0.0f --> no edge
                    imgOut_ji[0] = imgOut_ji[0] > threshold_1 ? 0.5f : 0.0f;
                }
            }
        }

        //remove false edges: a weak edge is a strong one
        //if it is connected to a strong edge
        int x[] = {1, 1, 0, -1, -1, -1,  0,  1};
        int y[] = {0, 1, 1,  1,  0, -1, -1, -1};

        for(int i = 0; i < imgOut->height; i++) {
            for(int j = 0; j < imgOut->width; j++) {
                float *imgOut_ji = (*imgOut)(j, i);

                if((imgOut_ji[0] > 0.4f) && (imgOut_ji[0] < 0.6f)) {
                    bool bRemove = true;

                    for(int k = 0; k < 8; k++) {
                        float *imgOut_ji_xy = (*imgOut)(j + x[k], i + y[k]);

                        if(imgOut_ji_xy[0] > 0.9f) {
                            bRemove = false;
                            break;
                        }
                    }

                    imgOut_ji[0] = bRemove ? 0.0f : 1.0f;
                }
            }
        }

        delete lum_flt;
        delete grad;

        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_FEATURES_MATCHING_CANNY_EDGE_DETECTOR_HPP */

