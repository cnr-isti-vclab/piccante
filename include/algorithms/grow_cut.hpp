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

#ifndef PIC_ALGORITHMS_GROW_CUT_HPP
#define PIC_ALGORITHMS_GROW_CUT_HPP

#include "../base.hpp"

#include "../image.hpp"
#include "../image_vec.hpp"

#include "../util/std_util.hpp"

#include "../filtering/filter_max.hpp"
#include "../filtering/filter_grow_cut.hpp"
#include "../filtering/filter_channel.hpp"

namespace pic {

class GrowCut
{
protected:
    FilterGrowCut flt;
    FilterMax *fltMax;
    Image *img_max, *state_next;

public:

    /**
     * @brief GrowCut
     */
    GrowCut()
    {
        state_next = NULL;
        img_max = NULL;

        fltMax = new FilterMax(5);
    }

    ~GrowCut()
    {
        delete_s(img_max);
        delete_s(fltMax);
        delete_s(state_next);
    }

    /**
     * @brief checkImage
     * @param img
     * @param width
     * @param height
     * @param channels
     * @return
     */
    static bool checkImage(Image *img, int width, int height, int channels)
    {
        return (img->channels != channels) ||
               (img->width    != width) ||
               (img->height   != height);
    }

    /**
     * @brief fromStrokeImageToSeeds
     * @param strokes
     * @param out
     * @return
     */
    static Image *fromStrokeImageToSeeds(Image *strokes, Image *out)
    {
        if(strokes == NULL) {
            return out;
        }

        if(strokes->channels < 3) {
            return out;
        }

        if(out == NULL) {
            out = new Image(1, strokes->width, strokes->height, 1);
        } else {
            if(checkImage(out, strokes->width, strokes->height, 1)) {
                out = new Image(1, strokes->width, strokes->height, 1);
            }
        }

        //red  --> +1
        //blue --> -1
        float red[]  = {1.0f, 0.0f, 0.0f};
        float blue[] = {0.0f, 0.0f, 1.0f};

        for(int i = 0; i < strokes->nPixels(); i++) {
            int ind = i * strokes->channels;

            float d_red  = sqrtf(Arrayf::distanceSq(red,  &strokes->data[ind], 3));
            float d_blue = sqrtf(Arrayf::distanceSq(blue, &strokes->data[ind], 3));

            out->data[i] = 0.0f;

            out->data[i] = d_red  < 0.5f ?  1.0f : out->data[i];
            out->data[i] = d_blue < 0.5f ? -1.0f : out->data[i];
        }

        return out;
    }

    /**
     * @brief getMaskAsImage
     * @param state
     * @return
     */
    static Image* getMaskAsImage(Image *state, Image *out)
    {
        if(state == NULL) {
            return out;
        }

        if(out == NULL) {
            out = new Image(1, state->width, state->height, 1);
        }

        return FilterChannel::execute(state, out, 0);
    }

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *Process(ImageVec imgIn, Image *imgOut)
    {
        if(!ImageVecCheck(imgIn, 2)) {
            return imgOut;
        }

        auto img = imgIn[0];
        auto seeds = imgIn[1];

        if(imgOut == NULL) {
            imgOut = new Image(img->width, img->height, 2);
        } else {
            if(checkImage(imgOut, img->width, img->height, 2)) {
                imgOut = new Image(img->width, img->height, 2);
            }
        }

        auto state_cur = imgOut;

        if(state_next == NULL) {
            state_next = state_cur->allocateSimilarOne();
        } else {
            if(checkImage(state_cur, img->width, img->height, 2)) {
                state_next = state_cur->allocateSimilarOne();
            }
        }

        //compute max
        img_max = fltMax->Process(Single(img), img_max);

        for(int i = 0; i < state_cur->nPixels(); i++) {
            //init state_cur
            int j  = i * state_cur->channels;
            int j_seeds = i * seeds->channels;
            state_cur->data[j] = seeds->data[j_seeds];
            state_cur->data[j + 1] = fabsf(seeds->data[j_seeds]) > 0.0f ? 1.0f : 0.0f;

            //fix max
            j = i * img_max->channels;
            img_max->data[j] = Arrayf::norm_sq(&img_max->data[j], img_max->channels);
        }

        //iterative filtering...
        int iterations = int(img->getDiagonalSize());

        if((iterations % 2) == 1) {
            iterations++;
        }

        ImageVec input = Triple(state_cur, img, img_max);
        Image *output = state_next;

        for(int i = 0; i < iterations; i++) {
            output = flt.Process(input, output);

            Image *tmp = input[0];
            input[0] = output;
            output = tmp;
        }

        return imgOut;
    }

    static Image *execute(Image *img, Image *seeds, Image *imgOut)
    {
        GrowCut gc;
        return gc.Process(Double(img, seeds), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_ALGORITHMS_GROW_CUT_HPP */

