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

//An dynamic array of Image*
typedef	std::vector<Image *> ImageVec;

/**Single: creates a single for filters input*/
PIC_INLINE ImageVec Single(Image *img)
{
    ImageVec ret;
    ret.push_back(img);
    return ret;
}

/**Double: creates a couple input for filters input*/
PIC_INLINE ImageVec Double(Image *img1, Image *img2)
{
    ImageVec ret;
    ret.push_back(img1);
    ret.push_back(img2);
    return ret;
}

/**Triple: creates a triple for filters input*/
PIC_INLINE ImageVec Triple(Image *img1, Image *img2, Image *img3)
{
    ImageVec ret;
    ret.push_back(img1);
    ret.push_back(img2);
    ret.push_back(img3);
    return ret;
}

/**Quad: creates a quaduple for filters input*/
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

} // end namespace pic

#endif /* PIC_IMAGE_RAW_VEC_HPP */

