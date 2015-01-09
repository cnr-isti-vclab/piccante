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

#include "image.hpp"
#include "image_samplers/image_sampler_bsplines.hpp"
#include "filtering/filter_down_pp.hpp"
#include "filtering/filter_up_pp.hpp"

namespace pic {

/**
 * @brief The PushPull class
 */
class PushPull
{
protected:

    FilterDownPP    *flt_down;
    FilterUpPP      *flt_up;

    float           threshold;
    float			*value;
    ImageVec 	stack;

    /**
     * @brief Release
     */
    void Release() {
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
        flt_down = NULL;
        flt_up = NULL;
        threshold = 1e-6f;
        value = NULL;
    }

    ~PushPull()
    {
        Release();
    }

    /**
     * @brief Process computes push-pull.
     * @param img
     * @param value
     * @return
     */
    Image *Process(Image *imgIn, Image *imgOut, float *value)
    {
        if(imgIn == NULL || value == NULL) {
            return NULL;
        }

        if(imgOut == NULL) {
            imgOut = imgIn->Clone();
        } else {
            imgOut->Assign(imgIn);
        }

        this->value = value;

        if(flt_down == NULL) {
            flt_down = new FilterDownPP(value, threshold);
        } else {
            flt_down->Update(value, threshold);
        }

        if(flt_up == NULL) {
            flt_up = new FilterUpPP(value, threshold);
        } else {
            flt_up->Update(value, threshold);
        }

        //creating the pyramid: Pull
        stack.push_back(imgOut);
        Image *work = imgOut;

        while(MIN(work->width, work->height) > 1) {
            Image *tmp = flt_down->Process(Single(work), NULL);

            if(tmp != NULL) {
                stack.push_back(tmp);
                work = tmp;
            }
        }

        //sampling from the pyramid (stack): Push
        int n = (stack.size() - 2);

        for(int i = n; i >= 0; i--) {
            stack[i] = flt_up->ProcessP(Single(stack[i + 1]), stack[i]);
        }

        return imgOut;
    }

    /**
     * @brief Execute
     * @param img
     * @param value
     * @return
     */
    static Image *Execute(Image *img, float value)
    {
        PushPull pp;

        float *tmpValue = new float[img->channels];
        for(int i = 0; i < img->channels; i++) {
            tmpValue[i] = value;
        }

        return pp.Process(img, NULL, tmpValue);
    }

    /**
     * @brief Execute
     * @param name
     * @param nameOut
     * @return
     */
    static Image *Execute(std::string name, std::string nameOut)
    {
        Image img(name);
        Image *imgOut = Execute(&img, 0.0f);
        imgOut->Write(nameOut);
        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_ALGORITHMS_PUSHPULL_HPP */

