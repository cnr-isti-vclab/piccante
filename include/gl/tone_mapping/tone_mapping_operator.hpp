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

#ifndef PIC_TONE_MAPPING_TONE_MAPPING_OPERATOR_GL_HPP
#define PIC_TONE_MAPPING_TONE_MAPPING_OPERATOR_GL_HPP

#include "../../util/std_util.hpp"

#include "../image.hpp"
#include "../image_vec.hpp"
#include "../filtering/filter_luminance.hpp"

namespace pic {

/**
 * @brief The ToneMappingOperator class
 */
class ToneMappingOperatorGL
{
protected:

    ImageGLVec images;

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     */
    virtual ImageGL *Process(ImageGL *imgIn, ImageGL *imgOut)
    {
        return imgOut;
    }

public:

    /**
     * @brief ToneMappingOperator
     */
    ToneMappingOperator()
    {

    }

    virtual void releaseAux()
    {

    }

    /**
     * @brief release
     */
    void release()
    {
        stdVectorClear<ImageGL>(images);
        releaseAux();
    }

    /**
     * @brief updateImage
     * @param imgIn
     */
    void updateImage(ImageGL *imgIn)
    {
        bool bRelease = false;
        for(auto i = 0; i < images.size(); i++) {
            if(images[i] != NULL) {
                if((imgIn->width  != images[i]->width) ||
                   (imgIn->height != images[i]->height)) {
                    bRelease = true;
                    break;
                }
            }
        }

        if(bRelease) {
            release();
        }
    }

    /**
     * @brief checkInput
     * @param imgIn
     * @return
     */
    bool checkInput(ImageGL *imgIn)
    {
        if(imgIn == NULL) {
            return false;
        }

        return imgIn->isValid();
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageGL *execute(ImageGL *imgIn, ImageGL *imgOut = NULL)
    {
        if(!checkInput(imgIn)) {
            return imgOut;
        }

        if(imgOut == NULL) {
            imgOut = imgIn->clone();
        } else {
            if(!imgOut->isSimilarType(imgIn)) {
                imgOut = imgIn->allocateSimilarOneGL();
            }
        }

        imgOut = Process(imgIn, imgOut);

        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_TONE_MAPPING_WARD_HISTOGRAM_TMO_HPP */

