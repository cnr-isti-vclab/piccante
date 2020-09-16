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
 * @brief The Stereo class
 */
class Stereo
{
protected:
    FilterLuminance flt_lum;
    FilterGradient  flt_grad;
    FilterDisparity flt_disp;

    int kernel_size, max_disparity, max_cross_check;

public:

    /**
     * @brief Stereo
     */
    Stereo()
    {
        init(7, 200, 8);
    }

    /**
     * @brief Stereo
     */
    Stereo(int kernel_size, int max_disparity, int max_cross_check)
    {
        init(kernel_size, max_disparity, max_cross_check);
    }

    /**
     * @brief init
     * @param kernel_size
     * @param max_disparity
     * @param max_cross_check
     */
    void init(int kernel_size, int max_disparity, int max_cross_check)
    {
        kernel_size = kernel_size > 0 ? kernel_size : 7;
        max_cross_check = max_cross_check > 0 ? max_cross_check : 4;

        this->kernel_size = kernel_size;
        this->max_disparity = max_disparity;
        this->max_cross_check = max_cross_check;

        flt_disp.update(max_disparity, kernel_size, 0.05f);
    }

    /**
     * @brief crossCheck
     * @param disp_left
     * @param disp_right
     */
    void crossCheck(Image *disp_left, Image *disp_right)
    {
        for(int i = 0; i < disp_left->height; i++) {

            for(int j = 0; j < disp_left->width; j++) {

                float *dL = (*disp_left)(j, i);

                if(dL[1] > 0.0f) { // if it is valid

                    int j_forward = int(dL[0]);
                    float *dR = (*disp_right)(j_forward, i);

                    if(dR[1] > 0.0f) { // if it is valid
                        int j_e = int(dR[0]);

                        if(std::abs(j - j_e) > max_cross_check) {
                            dL[0] = 0.0f;
                            dL[1] = -1.0f;
                        }
                    }
                }
            }
        }
    }

    /**
      * @brief computeLocalDisparity
      * @param disp
      */
    static void computeLocalDisparity(Image *disp)
    {
        for(int i = 0; i < disp->height; i++) {

            for(int j = 0; j < disp->width; j++) {
                float *tmp = (*disp)(j, i);

                tmp[0] -= float(j);
            }
        }
    }

    /**
     * @brief execute
     * @param img_left
     * @param img_right
     * @param disp_left
     * @param disp_right
     */
    void execute(Image *img_left, Image *img_right,
                 Image *disp_left, Image *disp_right)
    {
        if(img_left == NULL || img_right == NULL ||
           disp_left == NULL || disp_right == NULL) {
            return;
        }

        if(max_disparity < 0) {
            max_disparity = MIN(img_left->width, img_right->width) >> 1;
        }

        auto i_l_l = flt_lum.Process(Single(img_left), NULL);
        auto i_r_l = flt_lum.Process(Single(img_right), NULL);

        auto i_l_g = flt_grad.Process(Single(i_l_l), NULL);
        auto i_r_g = flt_grad.Process(Single(i_r_l), NULL);

        disp_left  = flt_disp.Process(Quad(img_left, img_right, i_l_g, i_r_g), disp_left);
        disp_right = flt_disp.Process(Quad(img_right, img_left, i_r_g, i_l_g), disp_right);

        crossCheck(disp_left, disp_right);
        crossCheck(disp_right, disp_left);

        computeLocalDisparity(disp_left);
        computeLocalDisparity(disp_right);
    }
};

} // end namespace pic

#endif // PIC_COMPUTER_VISION_STEREO_HPP
