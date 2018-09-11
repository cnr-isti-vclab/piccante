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

#ifndef PIC_COMPUTER_VISION_STEREO_HPP
#define PIC_COMPUTER_VISION_STEREO_HPP

#include <vector>
#include <random>
#include <stdlib.h>

#include "../base.hpp"

#include "../image.hpp"

#include "../filtering/filter_disparity.hpp"
#include "../filtering/filter_luminance.hpp"
#include "../filtering/filter_gradient.hpp"

#include "../util/math.hpp"

namespace pic {

/**
 * @brief checkDisparity
 * @param disp_left
 * @param disp_right
 * @param threshold
 */
PIC_INLINE void checkDisparity(Image *disp_left, Image *disp_right, int threshold = 8)
{
    for(int i = 0; i < disp_left->height; i++) {

        for(int j = 0; j < disp_left->width; j++) {

            float *dL = (*disp_left)(j, i);

            int j_forward = int(dL[0]);
            float *dR = (*disp_right)(j_forward, i);

            int j_e = int(dR[0]);

            if(std::abs(i - j_e) > threshold) {
                dL[0] = 0.0f;
                dL[1] = -1.0f;

                dR[0] = 0.0f;
                dR[1] = -1.0f;
            }
        }
    }
}

/**
  * @brief computeLocalDisparity
  * @param disp_left
  */
PIC_INLINE void computeLocalDisparity(Image *disp)
{
    for(int i = 0; i < disp->height; i++) {

        for(int j = 0; j < disp->width; j++) {
            float *tmp = (*disp)(j, i);

            tmp[0] -= float(j);
        }
    }
}

/**
 * @brief estimateStereo
 * @param img_left
 * @param img_right
 * @param max_disparity
 * @param disparity_cross_check
 * @param disp_left
 * @param disp_right
 */
PIC_INLINE void estimateStereo(Image *img_left, Image *img_right,
                               int max_disparity, int disparity_cross_check,
                               Image *disp_left, Image *disp_right)
{
    if(img_left  == NULL || img_right  == NULL ||
       disp_left == NULL || disp_right == NULL) {
        return;
    }

    if(max_disparity < 0) {
        max_disparity = MIN(img_left->width, img_right->width) >> 1;
    }

    if(disparity_cross_check < 0) {
        disparity_cross_check = 16;
    }

    auto i_l_l = FilterLuminance::Execute(img_left, NULL);
    auto i_r_l = FilterLuminance::Execute(img_right, NULL);

    auto i_l_g = FilterGradient::Execute(i_l_l, NULL);
    auto i_r_g = FilterGradient::Execute(i_r_l, NULL);

    FilterDisparity fd(240, 5);

    disp_left  = fd.ProcessP(pic::Quad(img_left, img_right, i_l_g, i_r_g), disp_left);
    disp_left  = fd.ProcessP(pic::Quad(img_right, img_left, i_r_g, i_l_g), disp_left);

    checkDisparity(disp_left, disp_right, disparity_cross_check);
    checkDisparity(disp_right, disp_left, disparity_cross_check);

    computeLocalDisparity(disp_left);
    computeLocalDisparity(disp_right);

}

} // end namespace pic

#endif // PIC_COMPUTER_VISION_STEREO_HPP
