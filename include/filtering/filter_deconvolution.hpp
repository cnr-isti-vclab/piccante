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

#ifndef PIC_FILTERING_FILTER_DECONVOLUTION_HPP
#define PIC_FILTERING_FILTER_DECONVOLUTION_HPP

#include "../base.hpp"

#include "../image.hpp"
#include "../filtering/filter_conv_2d.hpp"

namespace pic {

/**
 * @brief The FilterDeconvolution class
 */
class FilterDeconvolution: public Filter
{
protected:
    Image *psf_hat;
    Image *img_est_conv;
    Image *img_err;
    Image *img_rel_blur;
    FilterConv2D *flt_conv;

    int nIterations;

public:

    /**
     * @brief FilterDeconvolution
     * @param nIterations
     */
    FilterDeconvolution(int nIterations) : Filter()
    {
        minInputImages = 2;
        psf_hat = NULL;
        img_est_conv = NULL;
        img_err = NULL;
        img_rel_blur = NULL;
        flt_conv = new FilterConv2D();

        this->nIterations = 0;
        setup(nIterations);
    }

    /**
     * @brief setup
     * @param nIterations
     */
    void setup(int nIterations)
    {
        this->nIterations = nIterations > 0 ? nIterations : 16;
    }

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *Process(ImageVec imgIn, Image *imgOut)
    {
        if (!checkInput(imgIn)) {
            return imgOut;
        }

        imgOut = setupAux(imgIn, imgOut);

        if (imgOut == NULL) {
            return imgOut;
        }

        //
        //
        //

        Image *psf = imgIn[1];

        if(psf_hat == NULL) {
            psf_hat = psf->clone();
        } else {
            psf_hat->assign(psf);
        }

        psf_hat->flipHV();

        *imgOut = 0.5f;

        img_rel_blur = allocateOutputMemory(imgIn, img_rel_blur, true);
        img_est_conv = allocateOutputMemory(imgIn, img_est_conv, true);
        img_err = allocateOutputMemory(imgIn, img_err, true);

        ImageVec vec = Double(imgOut, psf);
        ImageVec vec_err = Double(img_rel_blur, psf_hat);

        for(int i = 0; i < nIterations; i++) {

            #ifdef PIC_DEBUG
                printf("%d\n", i);
            #endif

            img_est_conv = flt_conv->Process(vec, img_est_conv);

            img_rel_blur->assign(imgIn[0]);
            *img_rel_blur /= *img_est_conv;

            img_err = flt_conv->Process(vec_err, img_err);

            *imgOut *= *img_err;
        }

        return imgOut;
    }


    /**
     * @brief execute
     */
    static Image *execute(Image *imgIn, Image *psf, Image *imgOut, int nIterations)
    {
        FilterDeconvolution flt(nIterations);
        return flt.Process(Double(imgIn, psf), imgOut);
    }
};

}

#endif //PIC_FILTERING_FILTER_DECONVOLUTION_HPP
