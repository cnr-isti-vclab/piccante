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

#ifndef PIC_GL_ALGORITHMS_PUSHPULL_HPP
#define PIC_GL_ALGORITHMS_PUSHPULL_HPP

#include "../../gl/image.hpp"
#include "../../gl/image_vec.hpp"

#include "../../util/array.hpp"

#include "../../gl/filtering/filter_down_pp.hpp"
#include "../../gl/filtering/filter_up_pp.hpp"

namespace pic {

/**
 * @brief The PushPullGL class
 */
class PushPullGL
{
protected:

    FilterGLDownPP  *flt_down;
    FilterGLUpPP    *flt_up;

    ImageGLVec      stack;

    /**
     * @brief release
     */
    void release() {
        for(unsigned int i = 1; i < stack.size(); i++) {
            if(stack[i] != NULL){
                delete stack[i];
            }
        }

        stack.clear();
    }

public:

    /**
     * @brief PushPullGL
     */
    PushPullGL()
    {
        flt_down = NULL;
        flt_up = NULL;
    }

    ~PushPullGL()
    {
        release();
    }

    /**
     * @brief update
     * @param value
     * @param threshold
     */
    void update(float *value, float threshold = 1e-6f)
    {
        if(flt_down == NULL) {
            flt_down = new FilterGLDownPP(value, threshold);
        } else {
            flt_down->update(value, threshold);
        }

        if(flt_up == NULL) {
            flt_up = new FilterGLUpPP(value, threshold);
        } else {
            flt_up->update(value, threshold);
        }
    }

    /**
     * @brief Process computes push-pull.
     * @param img
     * @param value
     * @return
     */
    ImageGL *Process(ImageGL *imgIn, ImageGL *imgOut)
    {
        if(imgIn == NULL) {
            return imgOut;
        }

        if(imgOut == NULL) {
            imgOut = imgIn->cloneGL();
        } else {
            *imgOut = *imgIn;
        }

        ImageGL *work = imgOut;
        if(stack.empty()) { //create the pyramid: Pull
            stack.push_back(imgOut);

            while(MIN(work->width, work->height) > 1) {
                ImageGL *tmp = flt_down->Process(SingleGL(work), NULL);

                if(tmp != NULL) {
                    stack.push_back(tmp);
                    work = tmp;
                }
            }
        } else { //update previously created pyramid: Pull
            int c = 1;
            while(MIN(work->width, work->height) > 1) {
                flt_down->Process(DoubleGL(work, stack[c]), stack[c]);

                work = stack[c];
                c++;
            }
        }

        //sampling from the pyramid (stack): Push
        int n = int(stack.size() - 2);

        for(int i = n; i >= 0; i--) {
            flt_up->Process(DoubleGL(stack[i + 1], stack[i]), stack[i]);
        }

        return imgOut;
    }

    /**
     * @brief Execute
     * @param img
     * @param imgOut
     * @param value
     * @return
     */
    static ImageGL *execute(ImageGL *img,  ImageGL *imgOut, float value)
    {
        PushPullGL pp;

        float *tmp_value = new float[img->channels];
        Arrayf::assign(value, tmp_value, img->channels);

        pp.update(tmp_value, 1e-4f);
        imgOut = pp.Process(img, imgOut);

        delete[] tmp_value;

        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_GL_ALGORITHMS_PUSHPULL_HPP */

