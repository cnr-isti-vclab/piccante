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

#ifndef PIC_FEATURES_MATCHING_DENSE_SIFT_HPP
#define PIC_FEATURES_MATCHING_DENSE_SIFT_HPP

#include "util/array.hpp"

#include "util/rasterizer.hpp"

#include "filtering/filter_luminance.hpp"
#include "filtering/filter_gradient.hpp"
#include "filtering/filter_channel.hpp"
#include "filtering/filter_conv_2d.hpp"
#include "filtering/filter_conv_2dsp.hpp"

namespace pic {

class DenseSift
{
protected:

    float *GenKernel(int patch_size, int num_bins)
    {
        float r = float(patch_size) / 2.0f;
        float cx = float(r) - 0.5f;
        float sample_res = float(patch_size) / float(num_bins);

        float *weight = new float[patch_size];

        for(int i = 0; i < patch_size; i++) {
            float tmp = fabsf(float(i + 1) - cx) / sample_res;

            if(tmp <= 1.0f) {
                weight[i] = (1.0f - tmp);
            } else {
                weight[i] = 0.0f;
            }
        }

        return weight;
    }

    int		patch_size, half_patch_size;
    bool	bNormalization;
    bool	bLargeGradientsSupression;
    int		num_angles, num_bins, num_samples;
    float   CONST_GRADIENT_SUPRESSIO_THRESHOLD;

    Image *gauss, *L, *L_X, *L_Y, *gX, *gY, *grad, *I_orientation,
             *I_orientation_flt, *I_mag, *I_theta;
    int *shifter;
    float *cos_angles, *sin_angles;

    FilterConv2D fltConv;

public:

    DenseSift(int patch_size, bool bNormalization, bool bLargeGradientsSupression)
    {
        if(patch_size < 1) {
            patch_size = 8;
        }

        this->patch_size = patch_size;
        half_patch_size = (patch_size >> 1);

        this->bNormalization = bNormalization;
        this->bLargeGradientsSupression = bLargeGradientsSupression;

        CONST_GRADIENT_SUPRESSIO_THRESHOLD = 0.2f;

        gauss = new Image(1, 5, 5, 1);
        EvaluateGaussian(gauss, -1.0f, true);

        SetNULL();

        //precompute angles
        num_angles = 8;
        cos_angles = new float[num_angles];
        sin_angles = new float[num_angles];

        for(int a = 0; a < num_angles; a++) {
            float angle = (C_PI_2 * float(a)) / float(num_angles - 1);
            cos_angles[a] = cosf(angle);
            sin_angles[a] = sinf(angle);
        }

        //precompute fetching addresses
        num_bins = 4;
        num_samples = num_bins * num_bins;

        shifter = new int[num_bins];

        for(int i = 0; i < MIN(num_bins, patch_size); i++) {
            int tmp = ((patch_size + 1) * i) / (num_bins);
            shifter[i] = tmp - half_patch_size;
        }

    }

    ~DenseSift()
    {
        Destroy();
    }

    void SetNULL()
    {
        L = NULL;
        L_X = NULL;
        L_Y = NULL;
        gX = NULL;
        gY = NULL;
        I_orientation = NULL;
        grad = NULL;
        I_orientation_flt = NULL;
        I_mag = NULL;
        I_theta = NULL;
    }

    void Destroy()
    {
        if(L != NULL) {
            delete L;
        }

        if(L_X != NULL) {
            delete L_X;
        }

        if(L_Y != NULL) {
            delete L_Y;
        }

        if(gX != NULL) {
            delete gX;
        }

        if(gY != NULL) {
            delete gY;
        }

        if(I_orientation == NULL) {
            delete I_orientation;
        }

        if(I_orientation_flt != NULL) {
            delete I_orientation_flt;
        }

        if(grad != NULL) {
            delete grad;
        }

        if(shifter != NULL) {
            delete[] shifter;
        }

        if(cos_angles != NULL) {
            delete[] cos_angles;
        }

        if(sin_angles != NULL) {
            delete[] sin_angles;
        }
    }

    Image *get(Image *img, Image *sift_arr = NULL, float alpha = 9.0f)
    {
        if(img == NULL) {
            return NULL;
        }

        L = FilterLuminance::Execute(img, L, LT_CIE_LUMINANCE);
        float maxVal = L->getMaxVal()[0];
        *L /= maxVal;

        grad = FilterGradient::Execute(gauss, grad);

        gX = FilterChannel::Execute(grad, gX, 0);
        gY = FilterChannel::Execute(grad, gY, 1);

        float sX = 0.0f;
        float sY = 0.0f;

        for(int i = 0; i < gX->size(); i++) {
            sX += fabsf(gX->data[i]);
            sY += fabsf(gY->data[i]);
        }

        *gX *= (2.0f / sX);
        *gY *= (2.0f / sY);

        L_X = fltConv.ProcessP(Double(L, gX), L_X);
        L_Y = fltConv.ProcessP(Double(L, gY), L_Y);

        if(I_mag == NULL) {
            I_mag = gX;
        }

        if(I_theta == NULL) {
            I_theta = gY;
        }

        int size = L->width * L->height;

        for(int i = 0; i < size; i++) {
            float x = L_X->data[i];
            float y = L_Y->data[i];
            I_mag->data[i]   = sqrtf(x * x + y * y);
            I_theta->data[i] = atan2f(y, x);
        }

        //Calculate orientations
        int width = L->width;
        int height = L->height;

        if(I_orientation == NULL) {
            I_orientation = new Image(1, width, height, num_angles);
        }

        int size_n = size * num_angles;

        for(int i = 0; i < size_n; i += num_angles) {
            int j = i / num_angles;

            float cosI = cosf(I_theta->data[j]);
            float sinI = sinf(I_theta->data[j]);

            float tmp;

            for(int a = 0; a < num_angles; a++) {
                tmp = cosI * cos_angles[a] + sinI * sin_angles[a];
                tmp = MAX(powf(tmp, alpha), 0.0f);
                I_orientation->data[i + a] = tmp * I_mag->data[j];
            }
        }

        //Convolution of the orientations using a low-pass filter
        I_orientation_flt = FilterConv2DSP::Execute(I_orientation, I_orientation_flt,
                            GenKernel(patch_size, num_bins), patch_size);

        //Final dense sift
        if(sift_arr == NULL) {
            sift_arr = new Image(1, width, height, num_bins * num_bins * num_angles);
        }

        sift_arr->SetZero();

        for(int i = half_patch_size; i < (height - half_patch_size); i++) {
            for(int j = half_patch_size; j < (width - half_patch_size); j++) {
                float *sift_data = (*sift_arr)(j - half_patch_size, i - half_patch_size);

                int b = 0;

                for(int y = 0; y < num_bins; y++) {
                    for(int x = 0; x < num_bins; x++) {
                        float *I_ori_data = (*I_orientation_flt)(j + shifter[x], i + shifter[y]);

                        for(int k = 0; k < num_angles; k++) {
                            sift_data[b + k] = I_ori_data[k];
                        }

                        b += num_angles;
                    }
                }
            }
        }

        if(bNormalization) {
            Normalization(sift_arr);
        }

        return sift_arr;
    }

    void Normalization(Image *sift_arr) //normalziation of the sift
    {
        for(int i = 0; i < sift_arr->height; i++) {
            for(int j = 0; j < sift_arr->width; j++) {
                float *tmp_pixel = (*sift_arr)(j, i);

                //normalizing large gradients
                float norm = Array<float>::norm(tmp_pixel, sift_arr->channels);

                if(norm > 1.0f) {
                    Array<float>::normalize(tmp_pixel, sift_arr->channels, norm);

                    //supressing large gradients
                    if(bLargeGradientsSupression) {
                        for(int k = 0; k < sift_arr->channels; k++) {
                            tmp_pixel[k] = MIN(tmp_pixel[k], CONST_GRADIENT_SUPRESSIO_THRESHOLD);
                        }

                        Array<float>::normalize(tmp_pixel, sift_arr->channels);
                    }
                }
            }
        }
    }

};

} // end namespace pic

#endif /* PIC_FEATURES_MATCHING_DENSE_SIFT_HPP */

