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

#ifndef PIC_IMAGE_RAW_VEC_HPP
#define PIC_IMAGE_RAW_VEC_HPP

#include <vector>
#include "image.hpp"

namespace pic {

/**
 * @brief ImageVec an std::vector of pic::Image
 */
typedef	std::vector<Image *> ImageVec;

/**
 * @brief Single creates an std::vector which contains img; this is for filters input.
 * @param img is a pointer to a pic::Image
 * @return It returns an std::vector which contains img.
 */
PIC_INLINE ImageVec Single(Image *img)
{
    ImageVec ret;
    ret.push_back(img);
    return ret;
}

/**
 * @brief Double creates an std::vector which contains img1 and img2; this is for filters input.
 * @param img1 is a pointer to a pic::Image
 * @param img2 is a pointer to a pic::Image
 * @return It returns an std::vector which contains img1 and img2.
 */
PIC_INLINE ImageVec Double(Image *img1, Image *img2)
{
    ImageVec ret;
    ret.push_back(img1);
    ret.push_back(img2);
    return ret;
}

/**
 * @brief Triple creates an std::vector which contains img1, img2, and img3; this is for filters input.
 * @param img1 is a pointer to a pic::Image
 * @param img2 is a pointer to a pic::Image
 * @param img3 is a pointer to a pic::Image
 * @return It returns an std::vector which contains img1, img2, and img3.
 */
PIC_INLINE ImageVec Triple(Image *img1, Image *img2, Image *img3)
{
    ImageVec ret;
    ret.push_back(img1);
    ret.push_back(img2);
    ret.push_back(img3);
    return ret;
}

/**
 * @brief Triple creates an std::vector which contains img1, img2, img3, and img4; this is for filters input.
 * @param img1 is a pointer to a pic::Image
 * @param img2 is a pointer to a pic::Image
 * @param img3 is a pointer to a pic::Image
 * @param img4 is a pointer to a pic::Image
 * @return It returns an std::vector which contains img1, img2, img3, and img4.
 */
PIC_INLINE ImageVec Quad(Image *img1, Image *img2, Image *img3,
                            Image *img4)
{
    ImageVec ret;
    ret.push_back(img1);
    ret.push_back(img2);
    ret.push_back(img3);
    ret.push_back(img4);
    return ret;
}

/**
 * @brief ImaveVecSortByExposureTime
 * @param stack
 */
PIC_INLINE void ImaveVecSortByExposureTime(ImageVec &stack)
{
    std::sort(stack.begin(), stack.end(), [](const Image *l, const Image *r)->bool{
        if (!l || !r) {
            return false;
        }
        return l->exposure < r->exposure;
    });
}


/**
 * @brief ImaveVecGetExposureTimesAsArray
 * @param stack
 */
PIC_INLINE void ImaveVecGetExposureTimesAsArray(ImageVec &stack, std::vector<float> &output, bool bLog)
{
    output.clear();

    for(unsigned int i = 0; i < stack.size(); i++) {
        float tmp = bLog ? logf(stack[i]->exposure) : stack[i]->exposure;
        output.push_back(tmp);
    }
}

/**
 * @brief ImageVecCheckSimilarType
 * @param stack
 * @return
 */
PIC_INLINE bool ImageVecCheckSimilarType(ImageVec &stack)
{
    if(stack.size() < 2) {
        return false;
    }

    for (unsigned int i = 1; i < stack.size(); i++) {
        if (!stack[0]->isSimilarType(stack[i])) {
            return false;
        }
    }

    return true;
}

/**
 * @brief ImageVecCheck
 * @param vec
 * @param minInputImages
 * @return
 */
PIC_INLINE bool ImageVecCheck(ImageVec &imgIn, int minInputImages)
{
    int n;
    if(minInputImages < 0) {
        n = int(imgIn.size());
    } else {
        if(int(imgIn.size()) < minInputImages) {
            return false;
        }

        n = minInputImages;
    }

    for(int i = 0; i < n; i ++) {
        if(imgIn[i] == NULL) {
            return false;
        } else {
            if(!imgIn[i]->isValid()) {
                return false;
            }
        }
    }

    return true;
}

} // end namespace pic

#endif /* PIC_IMAGE_RAW_VEC_HPP */

