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

#include "gl/image.hpp"
#include "gl/filtering/filter_down_pp.hpp"
#include "gl/filtering/filter_up_pp.hpp"

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
     * @brief PushPullGL
     */
    PushPullGL()
    {
        flt_down = NULL;
        flt_up = NULL;
    }

    ~PushPullGL()
    {
        Release();
    }

    /**
     * @brief Process computes push-pull.
     * @param img
     * @param value
     * @return
     */
    ImageGL *Process(ImageGL *imgIn, ImageGL *imgOut, float *value, float threshold = 1e-6f)
    {
        if(imgIn == NULL) {
            return imgOut;
        }

        if(imgOut == NULL) {
            imgOut = imgIn->CloneGL();
        } else {
            *imgOut = *imgIn;
        }

        if(flt_down == NULL) {
            flt_down = new FilterGLDownPP(value, threshold);
        } else {
            flt_down->Update(value, threshold);
        }

        if(flt_up == NULL) {
            flt_up = new FilterGLUpPP(value, threshold);
        } else {
            flt_up->Update(value, threshold);
        }

        ImageGL *work = imgOut;
        if(stack.empty()) { //creating the pyramid: Pull
            stack.push_back(imgOut);

            while(MIN(work->width, work->height) > 1) {
                ImageGL *tmp = flt_down->Process(SingleGL(work), NULL);

                if(tmp != NULL) {
                    stack.push_back(tmp);
                    work = tmp;
                }
            }
        } else { //updating previously created pyramid: Pull
            int c = 1;
            while(MIN(work->width, work->height) > 1) {
                flt_down->Process(SingleGL(work), stack[c]);

                work = stack[c];
                c++;
            }
        }

        //sampling from the pyramid (stack): Push
        int n = (stack.size() - 2);

        for(int i = n; i >= 0; i--) {
            flt_up->Process(DoubleGL(stack[i + 1], stack[i]), stack[i]);
        }

        return imgOut;
    }

    /**
     * @brief Execute
     * @param img
     * @param value
     * @return
     */
    static ImageGL *Execute(ImageGL *img, float value)
    {
        PushPullGL pp;

        float *tmp_value = new float[img->channels];
        for(int i = 0; i < img->channels; i++) {
            tmp_value[i] = value;
        }

        return pp.Process(img, NULL, tmp_value);

        delete tmp_value;
    }

    /**
     * @brief Execute
     * @param name
     * @param nameOut
     * @return
     */
    static ImageGL *Execute(std::string name, std::string nameOut)
    {
        ImageGL img(name);
        ImageGL *imgOut = Execute(&img, 0.0f);
        imgOut->Write(nameOut);
        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_GL_ALGORITHMS_PUSHPULL_HPP */

