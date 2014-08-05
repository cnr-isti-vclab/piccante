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

#ifndef PIC_IMAGE_RAW_VEC_HPP
#define PIC_IMAGE_RAW_VEC_HPP

#include <vector>
#include "image_raw.hpp"

namespace pic {

//An dynamic array of ImageRAW*
typedef	std::vector<ImageRAW *> ImageRAWVec;

/**Single: creates a single for filters input*/
PIC_INLINE ImageRAWVec Single(ImageRAW *img)
{
    ImageRAWVec ret;
    ret.push_back(img);
    return ret;
}

/**Double: creates a couple input for filters input*/
PIC_INLINE ImageRAWVec Double(ImageRAW *img1, ImageRAW *img2)
{
    ImageRAWVec ret;
    ret.push_back(img1);
    ret.push_back(img2);
    return ret;
}

/**Triple: creates a triple for filters input*/
PIC_INLINE ImageRAWVec Triple(ImageRAW *img1, ImageRAW *img2, ImageRAW *img3)
{
    ImageRAWVec ret;
    ret.push_back(img1);
    ret.push_back(img2);
    ret.push_back(img3);
    return ret;
}

/**Quad: creates a quaduple for filters input*/
PIC_INLINE ImageRAWVec Quad(ImageRAW *img1, ImageRAW *img2, ImageRAW *img3,
                            ImageRAW *img4)
{
    ImageRAWVec ret;
    ret.push_back(img1);
    ret.push_back(img2);
    ret.push_back(img3);
    ret.push_back(img4);
    return ret;
}

} // end namespace pic

#endif /* PIC_IMAGE_RAW_VEC_HPP */

