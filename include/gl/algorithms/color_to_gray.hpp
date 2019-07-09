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

#ifndef PIC_GL_ALGORITHMS_COLOR_TO_GRAY_HPP
#define PIC_GL_ALGORITHMS_COLOR_TO_GRAY_HPP

#include "../../util/std_util.hpp"

#include "../../gl/image.hpp"
#include "../../gl/filtering/filter_channel.hpp"
#include "../../gl/tone_mapping/exposure_fusion.hpp"

namespace pic {

/**
 * @brief The ColorToGrayGL class
 */
class ColorToGrayGL
{
protected:
    FilterGLChannel  *flt;
    ExposureFusionGL *ef;
    ImageGLVec       img_vec;

public:
    /**
     * @brief ColorToGray
     */
    ColorToGrayGL()
    {
        flt = NULL;
        ef = NULL;
    }

    ~ColorToGrayGL()
    {
        delete_s(flt);
        delete_s(ef);
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageGL execute(ImageGL *imgIn, ImageGL *imgOut)
    {
        if(imgIn == NULL){
            return imgOut;
        }

        if(imgIn->channels != 3) {
            return imgOut;
        }

        if(imgOut == NULL){
            imgOut = new ImageGL(1, imgIn->width, imgIn->height, 1, IMG_GPU, GL_TEXTURE_2D);
        }

        ImageGLVec input = SingleGL(imgIn);

        if(flt == NULL) {
            flt = new FilterGLChannel(0);
        }

        int channels = imgIn->channels;

        if(img_vec.empty()) {
            for(int i = 0; i < channels; i++) {
                img_vec.push_back(flt->Process(input, NULL));
                flt->update(i + 1);
            }
        } else {
            for(int i = 0; i < channels; i++) {
                flt->Process(input, img_vec[i]);
                flt->update(i + 1);
            }
        }

        if(ef == NULL) {
            ef = new ExposureFusionGL(1.0f, 1.0f, 0.0f);
        }

        imgOut = ef->ProcessStack(img_vec, imgOut);

        return imgOut;
    }

};

} // end namespace pic

#endif /* PIC_GL_ALGORITHMS_COLOR_TO_GRAY_HPP */

