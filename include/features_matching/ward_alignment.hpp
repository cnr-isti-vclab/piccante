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

#ifndef PIC_FEATURES_MATCHING_WARD_ALIGNMENT_HPP
#define PIC_FEATURES_MATCHING_WARD_ALIGNMENT_HPP

#include <vector>

#include "../image.hpp"
#include "../util/vec.hpp"
#include "../util/string.hpp"
#include "../image_samplers/image_sampler_bilinear.hpp"
#include "../filtering/filter_downsampler_2d.hpp"
#include "../filtering/filter_luminance.hpp"

namespace pic {

/**
 * @brief The WardAlignment class
 */
class WardAlignment
{
protected:
    float tolerance, percentile;

public:
    ImageVec img1_v, img2_v, luminance;
    std::vector< bool* > tb1_v, tb2_v, eb2_shifted_v, tb2_shifted_v;

    /**
     * @brief WardAlignment
     */
    WardAlignment()
    {
        update(0.5f, 0.015625f);
    }

    ~WardAlignment()
    {
        for(unsigned int i=0; i< luminance.size(); i++) {
            delete luminance[i];
        }

        for(unsigned int i=0; i< img1_v.size(); i++) {
            delete img1_v[i];
        }

        for(unsigned int i=0; i< img2_v.size(); i++) {
            delete img2_v[i];
        }

        for(unsigned int i=0; i< tb1_v.size(); i++) {
            delete[] tb1_v[i];
        }

        for(unsigned int i=0; i< tb2_v.size(); i++) {
            delete[] tb2_v[i];
        }

        for(unsigned int i=0; i< eb2_shifted_v.size(); i++) {
            delete[] eb2_shifted_v[i];
        }

        for(unsigned int i=0; i<tb2_shifted_v.size(); i++) {
            delete[] tb2_shifted_v[i];
        }
    }

    /**
     * @brief update sets parameters up for MTB
     * @param percentile
     * @param tolerance
     */
    void update(float percentile, float tolerance)
    {
        if(percentile < 0.0f && percentile > 1.0f) {
            percentile = 0.5f;
        }

        if(tolerance > 0.0625f) {
            tolerance = 0.015625f;
        }

        this->percentile = percentile;
        this->tolerance = tolerance;
    }

    /**
     * @brief MTB computes the median threshold mask
     * @param img
     * @param L
     * @return
     */
    bool *MTB(Image *img, Image *L)
    {
        bool bDelete = (L == NULL);

        if(img->channels == 1)
        {
            bDelete = false;
            L = img;
        } else {
            L = FilterLuminance::execute(img, L, LT_WARD_LUMINANCE);
        }

        int n = L->nPixels();
        bool *maskThr = new bool[n * 2];
        bool *maskEb = &maskThr[n];

        float *ret = L->getPercentileVal(percentile, NULL, NULL);
        float medVal = ret[0];

        float A = medVal - tolerance;
        float B = medVal + tolerance;

        for(int i = 0; i < n; i++) {
            maskThr[i] = L->data[i] > medVal;
            maskEb[i]  = !((L->data[i] >= A) && (L->data[i] <= B));
        }

        if(bDelete) {
            delete L;
        }

        return maskThr;
    }

    /**
     * @brief getExpShift computes the shift vector for moving an img1 onto img2
     * @param img1
     * @param img2
     * @param shift_bits
     * @return
     */
    Vec2i getExpShift(Image *img1, Image *img2,
                                   int shift_bits = 6)
    {
        if(img1 == NULL || img2 == NULL) {
            return Vec2i(0, 0);
        }

        if(!img1->isSimilarType(img2)) {
            return Vec2i(0, 0);
        }

        Image *L1, *L2;

        if(img1->channels == 1) {
            L1 = img1;
        } else {
            L1 = FilterLuminance::execute(img1, NULL, LT_WARD_LUMINANCE);
            luminance.push_back(L1);
        }

        if(img2->channels == 1) {
            L2 = img2;
        } else {
            L2 = FilterLuminance::execute(img2, NULL, LT_WARD_LUMINANCE);
            luminance.push_back(L2);
        }

        int min_coord = MIN(L1->width, L1->height);
         if(min_coord < (1 << shift_bits)) {
             shift_bits = MAX(log2(min_coord) - 1, 1);
         }

        Vec2i cur_shift, ret_shift;

        cur_shift = Vec2i(0, 0);
        ret_shift = Vec2i(0, 0);

        //downsample
        Image *tmp_1 = L1;
        Image *tmp_2 = L2;
        for(int i = 0; i < shift_bits; i++) {
            Image* sml_img1 = FilterDownSampler2D::execute(tmp_1, NULL, 0.5f);
            Image* sml_img2 = FilterDownSampler2D::execute(tmp_2, NULL, 0.5f);

            img1_v.push_back(sml_img1);
            img2_v.push_back(sml_img2);

            tmp_1 = sml_img1;
            tmp_2 = sml_img2;
        }

        //compute the shift
        while(shift_bits > 0) {
            Image* sml_img1 = img1_v[shift_bits - 1];
            Image* sml_img2 = img2_v[shift_bits - 1];

            int width  = sml_img1->width;
            int height = sml_img1->height;
            int n = width * height;

             //compute the median threshold mask
            bool *tb1 = MTB(sml_img1, NULL);
            bool *eb1  = &tb1[n];

            bool *tb2 = MTB(sml_img2, NULL);
            bool *eb2  = &tb2[n];

            //track memory
            tb1_v.push_back(tb1);
            tb2_v.push_back(tb2);
            
            int min_err = n;

            bool *tb2_shifted = new bool[n];
            bool *eb2_shifted = new bool[n];

            tb2_shifted_v.push_back(tb2_shifted);
            eb2_shifted_v.push_back(eb2_shifted);

            for(int i = -1; i <= 1; i++) {

                for(int j = -1; j <= 1; j++) {

                    int xs = cur_shift[0] + i;
                    int ys = cur_shift[1] + j;

                    Buffer<bool>::shift(tb2_shifted, tb2, xs, ys, true, width, height, 1, 1);
                    Buffer<bool>::shift(eb2_shifted, eb2, xs, ys, true, width, height, 1, 1);

                    int err = 0;
                    for(int k=0; k<n; k++) {
                        bool diff_b = tb1[k] ^ tb2_shifted[k];
                        diff_b = diff_b & eb1[k];
                        diff_b = diff_b & eb2_shifted[k];

                        if(diff_b) {
                            err++;
                        }
                    }

                    if(err < min_err) {
                        ret_shift[0] = xs;
                        ret_shift[1] = ys;
                        min_err = err;
                    }
                }
            }

            shift_bits--;

            cur_shift[0] = ret_shift[0] * 2;
            cur_shift[1] = ret_shift[1] * 2;
        }

        return cur_shift;
    }

    static Vec2i execute(Image *imgTarget, Image *imgSource)
    {
        Vec2i shift;
        WardAlignment wa;

        if(imgTarget == NULL || imgSource == NULL) {
            return shift;
        }

        if(!imgTarget->isSimilarType(imgSource)) {
            return shift;
        }

        shift = wa.getExpShift(imgTarget, imgSource);

        return shift;
    }

    static Image *shiftImage(Image *img, Vec2i shift, Image *ret = NULL)
    {
        if(img == NULL) {
            return ret;
        }

        if(ret == NULL) {
            ret = img->allocateSimilarOne();
        } else {
            if(!ret->isSimilarType(img)) {
                ret = img->allocateSimilarOne();
            }
        }

        ret->setZero();
        Buffer<float>::shift(ret->data, img->data,
                             shift[0], shift[1], true,
                            img->width, img->height,
                            img->channels, img->frames);

        return ret;
    }


    /**
     * @brief execute aligns imgSource to imgTarget
     * @param imgTarget
     * @param imgSource
     * @param shift
     * @return
     */
    static Image *execute(Image *imgTarget, Image *imgSource, Vec2i &shift)
    {
        shift = execute(imgTarget, imgSource);

        if(shift[0] != 0 && shift[1] != 0) {
            Image *ret = shiftImage(imgSource, shift, NULL);
            return ret;
        } else {
            return imgSource;
        }
    }
};

} // end namespace pic

#endif /* PIC_FEATURES_MATCHING_WARD_ALIGNMENT_HPP */

