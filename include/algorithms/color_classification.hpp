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

#ifndef PIC_ALGORITHMS_COLOR_CLASSIFICATION_HPP
#define PIC_ALGORITHMS_COLOR_CLASSIFICATION_HPP

#include "../base.hpp"

#include "../util/math.hpp"

#include "../image.hpp"
#include "../filtering/filter_radial_basis_function.hpp"
#include "../filtering/filter_white_balance.hpp"
#include "../filtering/filter_luminance.hpp"
#include "../algorithms/lischinski_minimization.hpp"
#include "../util/mask.hpp"

namespace pic {

#ifndef PIC_DISABLE_EIGEN

/**
 * @brief computeColorClassification
 * @param img
 * @param white_pixel
 * @param color_samples
 * @param nSamples
 * @param nDim
 * @param variance_colors
 * @return
 */
PIC_INLINE bool *computeColorClassification(Image *img, float *white_pixel, float *color_samples, int nSamples, int nDim, float variance)
{
    if(color_samples == NULL || nSamples < 1 || nDim < 1 || variance <= 0.0f) {
        return NULL;
    }

    RadialBasisFunction rbf;
    rbf.update(color_samples, nSamples, nDim, variance);

    FilterRadialBasisFunction flt_rbf;
    flt_rbf.update(&rbf);

    FilterWhiteBalance flt_wb;
    flt_wb.update(white_pixel, img->channels, true);

    Image *img_wb = NULL;

    bool bFlag = true;
    if(white_pixel != NULL) {
       img_wb = FilterWhiteBalance::execute(img, white_pixel, NULL);
    } else {
        img_wb = img;
        bFlag = false;
    }

    #ifdef PIC_DEBUG
        img_wb->Write("../data/output/s_input_wb.bmp");
    #endif

    Image *img_wb_rbf = flt_rbf.Process(Single(img_wb), NULL);

    img_wb_rbf->clamp(0.0f, 1.0f);

    Image *img_L = FilterLuminance::execute(img, NULL);

    Image *opt = LischinskiMinimization(img_L, img_wb_rbf);

    float value = 1.0f;
    bool *mask = opt->convertToMask(&value, 0.25f, false, NULL);

    int width = opt->width;
    int height = opt->height;
    bool *tmp;
    tmp = Mask::dilate(NULL, mask, width, height, 3);
    Mask::dilate(mask, tmp, width, height, 3);
    Mask::dilate(tmp, mask, width, height, 3);
    Mask::dilate(mask, tmp, width, height, 3);

    Mask::removeIsolatedPixels(tmp, mask, width, height);

    Mask::erode(mask, tmp, width, height, 3);
    Mask::erode(tmp, mask, width, height, 3);
    Mask::erode(mask, tmp, width, height, 3);

    #ifdef PIC_DEBUG
        opt->convertFromMask(mask, width, height);
        opt->Write("../data/output/opt.bmp");
    #endif

    if(tmp != mask) {
        delete[] tmp;
    }

    delete opt;
    delete img_L;
    delete img_wb_rbf;

    if(bFlag) {
        delete img_wb;
    }

    return mask;
}
#endif

} // end namespace pic

#endif /* PIC_ALGORITHMS_COLOR_CLASSIFICATION_HPP */
