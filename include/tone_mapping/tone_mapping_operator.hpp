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

#ifndef PIC_TONE_MAPPING_TONE_MAPPING_OPERATOR_HPP
#define PIC_TONE_MAPPING_TONE_MAPPING_OPERATOR_HPP

#include "../image.hpp"
#include "../image_vec.hpp"
#include "../util/array.hpp"
#include "../filtering/filter_luminance.hpp"

namespace pic {

/**
 * @brief The ToneMappingOperator class
 */
class ToneMappingOperator
{
protected:

    ImageVec images;

    /**
     * @brief ProcessAux
     * @param imgIn
     * @param imgOut
     */
    virtual Image *ProcessAux(ImageVec imgIn, Image *imgOut)
    {
        return imgOut;
    }

    /**
     * @brief releaseAux
     */
    virtual void releaseAux()
    {

    }

public:

    /**
     * @brief ToneMappingOperator
     */
    ToneMappingOperator()
    {

    }

    /**
     * @brief release
     */
    void release()
    {
        stdVectorClear<Image>(images);
        releaseAux();
    }

    /**
     * @brief updateImage
     * @param imgIn
     */
    void updateImage(Image *imgIn)
    {
        bool bRelease = false;
        for(uint i = 0; i < images.size(); i++) {
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
     * @brief getScaleFiltering
     * @param imgIn
     * @param fx
     * @param fy
     */
    static void getScaleFiltering(Image *imgIn, int &fScaleX, int &fScaleY)
    {
        int maxCoord = MAX(imgIn->width, imgIn->height);

        float maxCoordf       = 2.0f * float(maxCoord) * 0.75f;
        float viewAngleWidth  = 2.0f * atanf(imgIn->width / maxCoordf);
        float viewAngleHeight = 2.0f * atanf(imgIn->height / maxCoordf);

        fScaleX = int((2.0f * tanf(viewAngleWidth / 2.0f) / 0.01745f));
        fScaleY = int((2.0f * tanf(viewAngleHeight / 2.0f) / 0.01745f));
    }

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *Process(ImageVec imgIn, Image *imgOut = NULL)
    {
        if(!ImageVecCheck(imgIn, -1)) {
            return imgOut;
        }

        if(imgOut == NULL) {
            imgOut = imgIn[0]->clone();
        } else {
            if(!imgOut->isSimilarType(imgIn[0])) {
                imgOut = imgIn[0]->allocateSimilarOne();
            }
        }

        imgOut = ProcessAux(imgIn, imgOut);

        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_TONE_MAPPING_WARD_HISTOGRAM_TMO_HPP */

