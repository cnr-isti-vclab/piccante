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

#ifndef PIC_GL_ALGORITHMS_RICHARDSON_LUCY_DECONVOLUTION_GL_HPP
#define PIC_GL_ALGORITHMS_RICHARDSON_LUCY_DECONVOLUTION_GL_HPP

#include "../../gl/image.hpp"

#include "../../gl/filtering/filter.hpp"
#include "../../gl/filtering/filter_conv_2d.hpp"

namespace pic {

class RichardsonLucyDeconvolutionGL
{
public:
    ImageGL *psf, *psf_hat;
    ImageGL *img_est_conv;
    ImageGL *img_err;
    ImageGL *img_rel_blur;

    FilterGLConv2D *flt_conv;
    int nIterations;

    RichardsonLucyDeconvolutionGL()
    {
        flt_conv = NULL;
        psf_hat = NULL;

        img_est_conv = NULL;
        img_err = NULL;
        img_rel_blur = NULL;
    }

    ~RichardsonLucyDeconvolutionGL()
    {
        if(flt_conv != NULL) {
            delete flt_conv;
        }

        if(psf_hat != NULL) {
            delete psf_hat;
        }

        if(img_est_conv != NULL) {
            delete img_est_conv;
        }

        if(img_err != NULL) {
            delete img_err;
        }

        if(img_rel_blur != NULL) {
            delete img_rel_blur;
        }
    }

    /**
     * @brief setup
     * @param psf
     * @param nIterations
     */
    void setup(ImageGL *psf, int nIterations)
    {
        nIterations = MAX(nIterations, 16);
        this->nIterations = nIterations;

        if(psf != NULL) {
            this->psf = psf;
            psf_hat = psf->cloneGL();
            psf_hat->flipHV();
        }
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageGL *execute(ImageGL *imgIn, ImageGL *imgOut)
    {
        if((imgIn == NULL) || (psf == NULL)) {
            return imgOut;
        }

        if(flt_conv == NULL) {
            flt_conv = new FilterGLConv2D(GL_TEXTURE_2D);
        }

        auto imgIn_vec = SingleGL(imgIn);
        /*
        imgOut = FilterGL::allocateOutputMemory(imgIn_vec, imgOut, false);
        img_est_conv = FilterGL::allocateOutputMemory(imgIn_vec, img_est_conv, true);
        img_err = FilterGL::allocateOutputMemory(imgIn_vec, img_err, true);
        img_rel_blur = FilterGL::allocateOutputMemory(imgIn_vec, img_rel_blur, true);
        */


        ImageGLVec vec = DoubleGL(imgOut, psf);
        ImageGLVec vec_err = DoubleGL(img_rel_blur, psf_hat);

        *imgOut = 0.5f;

        for(int i = 0; i < nIterations; i++) {

            #ifdef PIC_DEBUG
                printf("%d\n", i);
            #endif

            img_rel_blur = imgIn;

            img_est_conv = flt_conv->Process(vec, img_est_conv);
            *img_rel_blur /= *img_est_conv;

            img_err = flt_conv->Process(vec_err, img_err);

            *imgOut *= *img_err;
        }

        return imgOut;
    }

};

}

#endif //PIC_GL_ALGORITHMS_RICHARDSON_LUCY_DECONVOLUTION_GL_HPP
