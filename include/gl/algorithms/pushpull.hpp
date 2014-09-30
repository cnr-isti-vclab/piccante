/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

PICCANTE is free software; you can redistribute it and/or modify
under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 3.0 of
the License, or (at your option) any later version.

PICCANTE is distributed in the hope that it will be useful, but
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License
( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.

*/

#ifndef PIC_GL_ALGORITHMS_PUSHPULL_HPP
#define PIC_GL_ALGORITHMS_PUSHPULL_HPP

#include "gl/image_raw.hpp"

namespace pic {

/**
 * @brief The PushPull class
 */
class PushPullGL
{
protected:
    float			*value;
    bool            noMore;
    ImageRAWVec 	stack;
    float           threshold;

    //TODO: to create a filter out of this function
    /**
     * @brief up
     * @param imgUp
     * @param imgDown
     */
    void up(ImageRAW *imgUp, ImageRAW *imgDown)
    {
        ImageSamplerBSplines isb;
        float *vOut = new float[imgUp->channels];

        for(int i = 0; i < imgUp->height; i++) {
            float y = float(i) / imgUp->heightf;

            for(int j = 0; j < imgUp->width; j++) {
                float x = float(j) / imgUp->widthf;

                float *data = (*imgUp)(j, i);

                for(int k = 0; k < imgUp->channels; k++) {
                    if(data[k] == value[k]) {
                        isb.SampleImage(imgDown, x, y, vOut);
                        data[k] = vOut[k];
                    }
                }
            }
        }
    }

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
        threshold = 1e-4f;
        value = NULL;
        noMore = false;
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
    ImageRAW *Process(ImageRAW *imgIn, ImageRAW *imgOut, float *value)
    {
        if(imgIn == NULL || value == NULL) {
            return NULL;
        }

        if(imgOut == NULL) {
            imgOut = imgIn->Clone();
        }

        //creating the pyramid: Pull
        this->value = value;
        noMore = false;
        stack.push_back(imgOut);
        ImageRAW *work = imgOut;

        while(!noMore) {
            ImageRAW *tmp = down(work);

            if(tmp != NULL) {
                stack.push_back(tmp);
                work = tmp;
            }
        }

        //sampling from the pyramid (stack): Push
        int n = (stack.size() - 2);

        for(int i = n; i >= 0; i--) {
            up(stack[i], stack[i + 1]);
        }

        return imgOut;
    }

    /**
     * @brief Execute
     * @param img
     * @param value
     * @return
     */
    static ImageRAW *Execute(ImageRAW *img, float value)
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
    static ImageRAW *Execute(std::string name, std::string nameOut)
    {
        ImageRAW img(name);
        ImageRAW *imgOut = Execute(&img, 0.0f);
        imgOut->Write(nameOut);
        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_GL_ALGORITHMS_PUSHPULL_HPP */

