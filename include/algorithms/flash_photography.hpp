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
#include "filtering/filter.hpp"
#include "filtering/filter_bilateral_2df.hpp"

namespace pic {

/**
 * @brief FlashPhotography implements basic flash photography.
 * @param nameFlash
 * @param nameNoFlash
 * @param nameOut
 * @param filter
 * @return
 */
Image *FlashPhotography(std::string nameFlash, std::string nameNoFlash,
                           std::string nameOut,
                           Filter *filter)
{

    //Loading Images
    printf("Read flash image...");
    Image imgFlash(nameFlash);
    printf("ok\n");
    printf("Read no-flash image...");
    Image imgNoFlash(nameNoFlash);
    printf("ok\n");

    //Filter
    Image *imgOut = filter->ProcessP(Double(&imgNoFlash, &imgFlash), NULL);

    //Write image
    imgOut->Write(nameOut);

    return imgOut;
}

/**
 * @brief FlashPhotography2DF
 * @param nameFlash
 * @param nameNoFlash
 * @param nameOut
 * @param sigma_s
 * @param sigma_r
 * @return
 */
Image *FlashPhotography2DF(std::string nameFlash, std::string nameNoFlash,
                              std::string nameOut,
                              float sigma_s, float sigma_r)
{
    FilterBilateral2DF filter(sigma_s, sigma_r);
    return FlashPhotography(nameFlash, nameNoFlash, nameOut, (Filter *)&filter);
}

/**
 * @brief FlashPhotography2DS
 * @param nameFlash
 * @param nameNoFlash
 * @param nameOut
 * @param sigma_s
 * @param sigma_r
 * @return
 */
Image *FlashPhotography2DS(std::string nameFlash, std::string nameNoFlash,
                              std::string nameOut,
                              float sigma_s, float sigma_r)
{
    FilterBilateral2DS filter(sigma_s, sigma_r, 1);
    return FlashPhotography(nameFlash, nameNoFlash, nameOut, (Filter *)&filter);
}

} // end namespace pic

#endif /* PIC_ALGORITHMS_FLASH_PHOTOGRAPHY_HPP */

