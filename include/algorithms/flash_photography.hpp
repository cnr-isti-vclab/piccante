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

#ifndef PIC_ALGORITHMS_FLASH_PHOTOGRAPHY_HPP
#define PIC_ALGORITHMS_FLASH_PHOTOGRAPHY_HPP

#include "image.hpp"
#include "filtering/filter_bilateral_2df.hpp"

namespace pic {

/**
 * @brief flashPhotography
 * @param flash is a flash image that is linearized
 * @param no_flash is a no-flash image that is linearized
 * @param s_s spatial sigma of the bilateral filter in [5.0, 10.0]
 * @param s_r range sigma of the biltareal filter in [0.05, 0.1]
 * @return
 */
PIC_INLINE Image *flashPhotography(Image *flash, Image *no_flash, float s_s = 5.0f, float s_r = 0.05f)
{
    return NULL;
}

} // end namespace pic

#endif /* PIC_ALGORITHMS_FLASH_PHOTOGRAPHY_HPP */
