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

#ifndef PIC_ALGORITHMS_PUSHPULL_HPP
#define PIC_ALGORITHMS_PUSHPULL_HPP

#include "../image.hpp"
#include "../image_samplers/image_sampler_bsplines.hpp"
#include "../filtering/filter_down_pp.hpp"
#include "../filtering/filter_up_pp.hpp"

namespace pic {

/**
 * @brief The PushPull class
 */
class PushPull
{
protected:

    FilterDownPP *flt_down;
    FilterUpPP *flt_up;
    ImageVec stack;

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
     * @brief PushPull
     */
    PushPull()
    {

    }

    ~PushPull()
    {
        release();
    }

    /**
     * @brief update
     * @param value
     * @param threshold
     */
    void update(float *value, float threshold)
    {
        if(flt_down == NULL) {
            flt_down = new FilterDownPP(value, threshold);
        } else {
            flt_down->update(value, threshold);
        }

        if(flt_up == NULL) {
            flt_up = new FilterUpPP(value, threshold);
        } else {
            flt_up->update(value, threshold);
        }
    }

    /**
     * @brief process computes push-pull.
     * @param img
     * @param imgOut
     * @return
     */
    Image *Process(Image *imgIn, Image *imgOut)
    {
        if(imgIn == NULL) {
            return imgOut;
        }

        if(imgOut == NULL) {
            imgOut = imgIn->clone();
        } else {
            *imgOut = *imgIn;
        }

        Image *work = imgOut;

        if(stack.empty()) { //create the pyramid: Pull
            stack.push_back(imgOut);

            while(MIN(work->width, work->height) > 1) {
                Image *tmp = flt_down->Process(Single(work), NULL);

                if(tmp != NULL) {
                    stack.push_back(tmp);
                    work = tmp;
                }
            }
        } else { //update previously created pyramid: Pull
            int c = 1;
            while(MIN(work->width, work->height) > 1) {
                flt_down->Process(Double(work, stack[c]), stack[c]);
                work = stack[c];
                c++;
            }
        }

        //sample from the pyramid (stack): Push
        int n = int(stack.size()) - 2;

        for(int i = n; i >= 0; i--) {
            flt_up->Process(Double(stack[i + 1], stack[i]), stack[i]);
        }

        return imgOut;
    }

    /**
     * @brief execute
     * @param img
     * @param imgOut
     * @param value
     * @return
     */
    static Image *execute(Image *img, Image *imgOut, float value)
    {
        PushPull pp;

        float *tmp_value = new float[img->channels];
        for(int i = 0; i < img->channels; i++) {
            tmp_value[i] = value;
        }

        pp.update(tmp_value, 1e-4f);
        imgOut = pp.Process(img, imgOut);

        delete[] tmp_value;

        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_ALGORITHMS_PUSHPULL_HPP */

