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

#ifndef PIC_GL_ALGORITHMS_RICHARDSON_LUCY_DECONVOLUTION_HPP
#define PIC_GL_ALGORITHMS_RICHARDSON_LUCY_DECONVOLUTION_HPP

#include "gl/image.hpp"
#include "gl/filtering/filter_conv_2d.hpp"

namespace pic {

class RichardsonLucyDeconvolution
{
public:
    FilterGLConv2D *flt_conv;

    RichardsonLucyDeconvolution()
    {
        flt_conv = NULL;
    }

    ~RichardsonLucyDeconvolution()
    {
        delete flt_conv;
    }

    /**
     * @brief compute
     * @param imgIn
     * @param psf
     * @param nIterations
     * @param imgOut
     * @return
     */
    ImageGL *compute(ImageGL *imgIn, ImageGL *psf, int nIterations, ImageGL *imgOut);

};

PIC_INLINE ImageGL *RichardsonLucyDeconvolution::compute(ImageGL *imgIn, ImageGL *psf, int nIterations = 10, ImageGL *imgOut = NULL)
{
    if((imgIn == NULL) || (psf == NULL)) {
        return imgOut;
    }

    if(flt_conv == NULL) {
        flt_conv = new FilterGLConv2D(GL_TEXTURE_2D);
    }

    if(imgOut == NULL) {
        imgOut = imgIn->allocateSimilarOneGL();
    }

    if(nIterations < 1) {
        nIterations = 10;
    }

    ImageGL *psf_hat = psf->cloneGL();
    psf_hat->flipHV();

    *imgOut = 0.5f;

    ImageGL *img_rel_blur = imgIn->allocateSimilarOneGL();

    ImageGL *img_est_conv = NULL;
    ImageGL *img_err = NULL;

    ImageGLVec vec = DoubleGL(imgOut, psf);
    ImageGLVec vec_err = DoubleGL(img_rel_blur, psf_hat);

    for(int i = 0; i < nIterations; i++) {

        #ifdef PIC_DEBUG
            printf("%d\n", i);
        #endif

        img_est_conv = flt_conv->Process(vec, img_est_conv);

        //img_rel_blur->assignGL(imgIn);

        *img_rel_blur /= *img_est_conv;

        img_err = flt_conv->Process(vec_err, img_err);

        *imgOut *= *img_err;
    }

    return imgOut;
}

}

#endif //PIC_GL_ALGORITHMS_RICHARDSON_LUCY_DECONVOLUTION_HPP
