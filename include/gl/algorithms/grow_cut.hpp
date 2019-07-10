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

#ifndef PIC_GL_ALGORITHMS_GROW_CUT_HPP
#define PIC_GL_ALGORITHMS_GROW_CUT_HPP

#include "../../base.hpp"

#include "../../util/std_util.hpp"

#include "../../gl/image.hpp"
#include "../../gl/filtering/filter_max.hpp"
#include "../../gl/filtering/filter_grow_cut.hpp"
#include "../../gl/filtering/filter_channel.hpp"

namespace pic {

class GrowCutGL
{
protected:
    FilterGLGrowCut *flt;
    FilterGLMax *fltMax;
    ImageGL *img_max, *state_next;

public:

    /**
     * @brief GrowCutGL
     */
    GrowCutGL()
    {
        flt = NULL;
        fltMax = NULL;
        img_max = NULL;
        state_next = NULL;
    }

    ~GrowCutGL()
    {
        delete_s(flt);
        delete_s(fltMax);
        delete_s(img_max);
        delete_s(state_next);
    }

    /**
     * @brief fromStrokeImageToSeeds
     * @param strokes
     * @param out
     * @return
     */
    static Image *fromStrokeImageToSeeds(Image *strokes, Image *out)
    {
        if(strokes->channels < 3) {
            return out;
        }

        if(out == NULL) {
            out = new Image(1, strokes->width, strokes->height, 1);
        }

        //red  --> +1
        //blue --> -1
        float red[] = {1.0f, 0.0f, 0.0f};
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
     * @brief execute
     * @param img
     * @param seeds
     * @param state_cur
     * @return
     */
    ImageGL *execute(ImageGL *img, ImageGL *seeds, ImageGL *state_cur = NULL)
    {
        if(img == NULL || seeds == NULL) {
            return NULL;
        }

        if(state_cur == NULL) {
            state_cur = new ImageGL(1, img->width, img->height, 2, IMG_GPU, GL_TEXTURE_2D);
        }

        if(fltMax == NULL) {
            fltMax = new FilterGLMax(5);
        }

        if(flt == NULL) {
            flt = new FilterGLGrowCut();
        }

        ImageGL *state_next = state_cur->allocateSimilarOneGL();

        //compute max
        img_max = fltMax->Process(SingleGL(img), img_max);

        for(int i = 0; i < state_cur->nPixels(); i++) {
            //init state_cur
            int j  = i * state_cur->channels;
            int j2 = i * seeds->channels;
            state_cur->data[j] = seeds->data[j2];
            state_cur->data[j + 1] = fabsf(seeds->data[j2]) > 0.0f ? 1.0f : 0.0f;
        }

        //iterative filtering...
        int iterations = int(img->getDiagonalSize());

        if((iterations % 2) == 1) {
            iterations++;
        }

        ImageGLVec input = TripleGL(state_cur, img, img_max);
        ImageGL *output = state_next;

        for(int i = 0; i < iterations; i++) {
            output = flt->Process(input, output);

            ImageGL *tmp = input[0];
            input[0] = output;
            output = tmp;
        }

        return input[0];
    }

};

} // end namespace pic

#endif /* PIC_GL_ALGORITHMS_GROW_CUT_HPP */

