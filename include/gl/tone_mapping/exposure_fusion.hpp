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

#ifndef PIC_GL_TONE_MAPPING_EXPOSURE_FUSION_TMO_HPP
#define PIC_GL_TONE_MAPPING_EXPOSURE_FUSION_TMO_HPP

#include "../../util/math.hpp"

#include "../../util/std_util.hpp"

#include "../../gl/tone_mapping/get_all_exposures.hpp"

#include "../../gl/algorithms/pyramid.hpp"
#include "../../gl/filtering/filter_luminance.hpp"
#include "../../gl/filtering/filter_exposure_fusion_weights.hpp"
#include "../../gl/filtering/filter_op.hpp"

namespace pic {

/**
 * @brief The ExposureFusionGL class
 */
class ExposureFusionGL
{
protected:
    std::vector<FilterGL *> filters;
    std::vector<PyramidGL *> pyramids;
    ImageGLVec images;

    FilterGLLuminance *flt_lum;
    FilterGLExposureFusionWeights *flt_weights;
    FilterGLOp *removeNegative;

    ImageGL *lum, *acc, *weights;

    PyramidGL *pW, *pI, *pOut;

    float wC, wS, wE;

    bool bAllocatedFilters;
    bool bAllocate;

    /**
     * @brief allocateFilters
     */
    void allocateFilters()
    {
        if(!bAllocatedFilters) {
            flt_lum = new FilterGLLuminance();
            flt_lum->update(LT_LUMA);
            flt_lum->bDelete = true;
            filters.push_back(flt_lum);

            removeNegative = new FilterGLOp("max(I0, vec4(0.0))", true, NULL, NULL);
            removeNegative->bDelete = true;
            filters.push_back(removeNegative);

            flt_weights = new FilterGLExposureFusionWeights(wC, wE, wS);
            flt_weights->bDelete = true;
            filters.push_back(flt_weights);
            bAllocatedFilters = true;
        }
    }

public:
    /**
     * @brief ExposureFusionGL
     */
    ExposureFusionGL(float wC = 1.0f, float wE = 1.0f, float wS = 1.0f)
    {
        bAllocate = false;
        bAllocatedFilters = false;

        update(wC, wE, wS);

        lum = NULL;
        acc = NULL;
        weights = NULL;

        pW = NULL;
        pI = NULL;
        pOut = NULL;
    }

    ~ExposureFusionGL()
    {
        stdVectorClear<ImageGL>(images);
        stdVectorClear<PyramidGL>(pyramids);
        stdVectorClear<FilterGL>(filters);
    }

    /**
     * @brief update
     * @param wC weight for preserving contrast
     * @param wE weight for preserving exposure
     * @param wS weight for preserving saturation
     */
    void update(float wC = 1.0f, float wE = 1.0f, float wS = 1.0f)
    {
        this->wC = CLAMPi(wC, 0.0f, 1.0f);
        this->wE = CLAMPi(wE, 0.0f, 1.0f);
        this->wS = CLAMPi(wS, 0.0f, 1.0f);
    }

    /**
     * @brief allocate
     * @param imgIn
     */
    void allocate(ImageGLVec imgIn)
    {
        if(!bAllocate) {

            int width = imgIn[0]->width;
            int height = imgIn[0]->height;
            int channels = imgIn[0]->channels;

            acc = new ImageGL(1, width, height, 1, IMG_GPU, GL_TEXTURE_2D);
            lum = new ImageGL(1, width, height, 1, IMG_GPU, GL_TEXTURE_2D);
            weights = new ImageGL(1, width, height, 1, IMG_GPU, GL_TEXTURE_2D);

            images.push_back(acc);
            images.push_back(lum);
            images.push_back(weights);

            int limitLevel = 2;
            pW = new PyramidGL(width, height, 1, false, limitLevel);
            pI = new PyramidGL(width, height, channels, true, limitLevel);
            pOut = new PyramidGL(width, height, channels, true, limitLevel);

            pyramids.push_back(pW);
            pyramids.push_back(pI);
            pyramids.push_back(pOut);

            allocateFilters();

            bAllocate = true;
        } else {
            if(!pOut->stack[0]->isSimilarType(imgIn[0])) {
                bAllocate = false;

                stdVectorClear<ImageGL>(images);
                stdVectorClear<PyramidGL>(pyramids);

                allocate(imgIn);
            }
        }
    }

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageGL *Process(ImageGL *imgIn, ImageGL *imgOut)
    {
        pic::ImageGLVec img_vec = getAllExposuresImagesGL(imgIn, 2.2f);

        imgOut = ProcessStack(img_vec, imgOut);

        stdVectorClear<ImageGL>(img_vec);

        return imgOut;
    }

    /**
     * @brief ProcessStack
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageGL *ProcessStack(ImageGLVec imgIn, ImageGL *imgOut = NULL)
    {
        auto n = imgIn.size();

        if(n < 2) {
            return imgOut;
        }

        allocate(imgIn);

        //compute weights values
        *acc = 0.0f;
        for(uint j = 0; j < n; j++) {
            lum = flt_lum->Process(SingleGL(imgIn[j]), lum);
            weights = flt_weights->Process(DoubleGL(lum, imgIn[j]), weights);

            *acc += *weights;
        }

        //accumulate on a pyramid
        pOut->setValue(0.0f);

        for(uint j = 0; j < n; j++) {
            lum = flt_lum->Process(SingleGL(imgIn[j]), lum);
            weights = flt_weights->Process(DoubleGL(lum, imgIn[j]), weights);

            //normalization
            *weights /= *acc;

            pW->update(weights);
            pI->update(imgIn[j]);

            pI->mul(pW);
            pOut->add(pI);
        }

        //final result
        imgOut = pOut->reconstruct(imgOut);

        float *minVal = imgOut->getMinVal(NULL);
        float *maxVal = imgOut->getMaxVal(NULL);

        int ind;
        float minV = Arrayf::getMin(minVal, imgOut->channels, ind);
        float maxV = Arrayf::getMax(maxVal, imgOut->channels, ind);
        *imgOut -= minV;
        *imgOut /= (maxV - minV);
        imgOut = removeNegative->Process(SingleGL(imgOut), imgOut);

        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_GL_TONE_MAPPING_EXPOSURE_FUSION_TMO_HPP */

