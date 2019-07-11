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
#include "../../gl/image_vec.hpp"

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
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageGL *Process(ImageGLVec imgIn, ImageGL *imgOut = NULL)
    {
        if(!ImageGLVecCheck(imgIn, 2)) {
            return imgOut;
        }

        auto img = imgIn[0];
        auto seeds = imgIn[1];

        if(imgOut == NULL) {
            imgOut = new ImageGL(1, img->width, img->height, 2, IMG_GPU, GL_TEXTURE_2D);
        }

        if(fltMax == NULL) {
            fltMax = new FilterGLMax(5);
        }

        if(flt == NULL) {
            flt = new FilterGLGrowCut();
        }

        auto state_cur = imgOut;

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

