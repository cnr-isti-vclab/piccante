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

#ifndef PIC_ALGORITHMS_COMPUTE_DIVERGENCE_HPP
#define PIC_ALGORITHMS_COMPUTE_DIVERGENCE_HPP

#include "../base.hpp"

#include "../util/std_util.hpp"

#include "../filtering/filter_conv_1d.hpp"

namespace pic {

/**
 * @brief DivergenceOperator calculates divergence of the gradient of an image.
 * @param img is an input image.
 * @param div is the output divergence of the gradient of img; i.e. Laplacian.
 * @return
 */
class DivergenceOperator
{
protected:
    FilterConv1D flt;
    float kernelGrad[3];
    float kernelDiv[3];
    Image *img_dx, *img_dy;

public:

    /**
     * @brief DivergenceOperator
     */
    DivergenceOperator()
    {
        kernelGrad[0] = -0.5f;
        kernelGrad[1] =  0.0f;
        kernelGrad[2] =  0.5f;

        kernelDiv[0] = -1.0f;
        kernelDiv[1] =  1.0f;
        kernelDiv[2] =  0.0f;

        img_dx = NULL;
        img_dy = NULL;
    }

    ~DivergenceOperator()
    {
        delete_s(img_dx);
        delete_s(img_dy);
    }

    /**
     * @brief Process
     * @param imgIn
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
            if(!imgOut->isSimilarType(imgIn)) {
                imgOut = imgIn->allocateSimilarOne();
            }
        }

        //compute gradient dx
        flt.update(kernelGrad, 3, true);
        img_dx = flt.Process(Single(imgIn), img_dx);

        //compute gradient dy
        flt.update(kernelGrad, 3, false);
        img_dy = flt.Process(Single(imgIn), img_dy);

        //compute the divergence using backward differences
        flt.update(kernelDiv, 3, true);
        imgOut = flt.Process(Single(img_dx), imgOut);

        flt.update(kernelDiv, 3, false);
        auto img_dy2 = flt.Process(Single(img_dy), img_dx);

        *imgOut += *img_dy2;

        return imgOut;
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut)
    {
        DivergenceOperator divOp;
        return divOp.Process(imgIn, imgOut);
    }
};

} // end namespace pic

#endif /* PIC_ALGORITHMS_COMPUTE_DIVERGENCE_HPP */

