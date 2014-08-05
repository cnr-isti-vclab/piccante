/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

PICCANTE is free software; you can redistribute it and/or modify
under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 3.0 of
the License, or (at your option) any later version.

PICCANTE is distributed in the hope that it will be useful, but
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License
( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.

*/

#ifndef PIC_GL_IMAGE_RAW_VEC_HPP
#define PIC_GL_IMAGE_RAW_VEC_HPP

#include <vector>

#include "gl/image_raw.hpp"

namespace pic {

/**
 * @brief ImageRAWGLVec an std::vector of pic::ImageRAWGL
 */
typedef	std::vector<ImageRAWGL *> ImageRAWGLVec;

/**
 * @brief SingleGL creates a single for filters input.
 * @param img
 * @return
 */
ImageRAWGLVec SingleGL(ImageRAWGL *img)
{
    ImageRAWGLVec ret;
    ret.push_back(img);
    return ret;
}

/**
 * @brief DoubleGL creates a couple for filters input.
 * @param img1
 * @param img2
 * @return
 */
ImageRAWGLVec DoubleGL(ImageRAWGL *img1, ImageRAWGL *img2)
{
    ImageRAWGLVec ret;
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
ImageRAWGLVec TripleGL(ImageRAWGL *img1, ImageRAWGL *img2, ImageRAWGL *img3)
{
    ImageRAWGLVec ret;
    ret.push_back(img1);
    ret.push_back(img2);
    ret.push_back(img3);
    return ret;
}

} // end namespace pic

#endif /* PIC_GL_IMAGE_RAW_VEC_HPP */

