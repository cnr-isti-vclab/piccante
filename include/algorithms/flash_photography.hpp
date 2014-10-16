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

#ifndef PIC_ALGORITHMS_FLASH_PHOTOGRAPHY_HPP
#define PIC_ALGORITHMS_FLASH_PHOTOGRAPHY_HPP

#include "image.hpp"
#include "filtering/filter.hpp"
#include "filtering/filter_bilateral_2df.hpp"

namespace pic {

/**FlashPhotography: implements flash photography*/
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

Image *FlashPhotography2DF(std::string nameFlash, std::string nameNoFlash,
                              std::string nameOut,
                              float sigma_s, float sigma_r)
{
    FilterBilateral2DF filter(sigma_s, sigma_r);
    return FlashPhotography(nameFlash, nameNoFlash, nameOut, (Filter *)&filter);
}

Image *FlashPhotography2DS(std::string nameFlash, std::string nameNoFlash,
                              std::string nameOut,
                              float sigma_s, float sigma_r)
{
    FilterBilateral2DS filter(sigma_s, sigma_r, 1);
    return FlashPhotography(nameFlash, nameNoFlash, nameOut, (Filter *)&filter);
}

} // end namespace pic

#endif /* PIC_ALGORITHMS_FLASH_PHOTOGRAPHY_HPP */

