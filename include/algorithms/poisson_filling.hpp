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

#ifndef PIC_ALGORITHMS_POISSON_FILLING_HPP
#define PIC_ALGORITHMS_POISSON_FILLING_HPP

#include "util/buffer.hpp"
#include "util/mask.hpp"
#include "image.hpp"

namespace pic {

/**
 * @brief The PoissonFilling class
 */
class PoissonFilling
{
protected:
    int			maxIter;
    float		threshold, value;

    bool		*mask;
    bool		*maskPoisson;
    Image       *imgTmp;


    /**
     * @brief release
     */
    void release()
    {
        if(mask != NULL) {
            delete[] mask;
            mask = NULL;
        }

        if(maskPoisson != NULL) {
            delete[] maskPoisson;
            maskPoisson = NULL;
        }

        if(imgTmp != NULL) {
            delete imgTmp;
            imgTmp = NULL;
        }
    }

public:

    /**
     * @brief PoissonFilling
     */
    PoissonFilling()
    {
        imgTmp = NULL;
        mask = NULL;
        maskPoisson = NULL;

        value = 0.0f;
        threshold = 1e-4f;

        maxIter = 1000;
    }

    ~PoissonFilling()
    {
        release();
    }

    /**
     * @brief update
     * @param imgOut
     * @param imgIn
     */
    void update(Image *imgOut, Image *imgIn)
    {
        imgOut->assign(imgIn);

        #pragma omp parallel for

        for(int i = 0; i < imgIn->height; i++) {
            for(int j = 0; j < imgIn->width; j++) {
                float *src =	(*imgIn)(j, i);
                float *n0  =	(*imgIn)(j + 1, i);
                float *n1  =	(*imgIn)(j - 1, i);
                float *n2  =	(*imgIn)(j, i + 1);
                float *n3  =	(*imgIn)(j, i - 1);

                int ind = i * imgIn->width + j;

                float *out = (*imgOut)(j, i);

                for(int k = 0; k < imgIn->channels; k++) {

                    if(equalf(src[k], value)) {
                        int div = 0;

                        out[k] = 0.0f;

                        if(!equalf(n0[k], value)) {
                            out[k] += n0[k];
                            div++;
                        }

                        if(!equalf(n1[k], value)) {
                            out[k] += n1[k];
                            div++;
                        }

                        if(!equalf(n2[k], value)) {
                            out[k] += n2[k];
                            div++;
                        }

                        if(!equalf(n3[k], value)) {
                            out[k] += n3[k];
                            div++;
                        }

                        if(div > 0) {
                            out[k] = out[k] / float(div);
                            mask[ind] = false;
                        }
                    } else { //Poisson solver
                        if(maskPoisson[ind]) {
                            int div = 0;
                            float tmp = 0.0f;

                            if(!equalf(n0[k], value)) {
                                tmp += -src[k] + n0[k];
                                div++;
                            }

                            if(!equalf(n1[k], value)) {
                                tmp += -src[k] + n1[k];
                                div++;
                            }

                            if(!equalf(n2[k], value)) {
                                tmp += -src[k] + n2[k];
                                div++;
                            }

                            if(!equalf(n3[k], value)) {
                                tmp += -src[k] + n3[k];
                                div++;
                            }

                            out[k] = src[k] + tmp / float(div);
                        }
                    }
                }
            }
        }
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param value
     * @return
     */
    Image *execute(Image *imgIn, Image *imgOut, float value)
    {
        if(imgIn == NULL) {
            return NULL;
        }

        if(!imgIn->isValid()) {
            return NULL;
        }

        if(imgTmp != NULL) {
            if(!imgTmp->isSimilarType(imgIn)) {
                release();
                imgTmp = imgIn->clone();
            }
        } else {
            imgTmp = imgIn->clone();
        }

        if(imgOut == NULL) {
            imgOut = imgIn->clone();
        } else {
            imgOut->assign(imgIn);
        }

        this->value = value;

        float *color = new float[imgIn->channels];

        for(int i = 0; i < imgIn->channels; i++) {
            color[i] = value;
        }

        mask = imgIn->convertToMask(color, threshold, false);

        maskPoisson = MaskClone(mask, maskPoisson, imgIn->width, imgIn->height);

        Image *work[2];
        work[0] = imgTmp;
        work[1] = imgOut;

        int i = 0;

        while(!MaskEmpty(mask, imgIn->width, imgIn->height)) {
            update(work[i % 2], work[(i + 1) % 2]);
            i++;

            if(i > maxIter) {
                break;
            }
        }

        if((i % 2) == 1) {
            imgOut->assign(imgTmp);
        }

        delete[] color;
        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_ALGORITHMS_POISSON_FILLING_HPP */

