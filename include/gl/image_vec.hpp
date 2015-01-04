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

#ifndef PIC_GL_IMAGE_RAW_VEC_HPP
#define PIC_GL_IMAGE_RAW_VEC_HPP

#include <vector>

#include "gl/image.hpp"

namespace pic {

/**
 * @brief ImageGLVec an std::vector of pic::ImageGL
 */
typedef	std::vector<ImageGL*> ImageGLVec;

/**
 * @brief SingleGL creates a single for filters input.
 * @param img
 * @return
 */
ImageGLVec SingleGL(ImageGL *img)
{
    ImageGLVec ret;
    ret.push_back(img);
    return ret;
}

/**
 * @brief DoubleGL creates a couple for filters input.
 * @param img1
 * @param img2
 * @return
 */
ImageGLVec DoubleGL(ImageGL *img1, ImageGL *img2)
{
    ImageGLVec ret;
    ret.push_back(img1);
    ret.push_back(img2);
    return ret;
}

/**
 * @brief TripleGL creates a triple for filters input.
 * @param img1
 * @param img2
 * @param img3
 * @return
 */
ImageGLVec TripleGL(ImageGL *img1, ImageGL *img2, ImageGL *img3)
{
    ImageGLVec ret;
    ret.push_back(img1);
    ret.push_back(img2);
    ret.push_back(img3);
    return ret;
}

} // end namespace pic

#endif /* PIC_GL_IMAGE_RAW_VEC_HPP */

