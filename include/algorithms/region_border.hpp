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

#ifndef PIC_ALGORITHMS_REGION_BORDER_HPP
#define PIC_ALGORITHMS_REGION_BORDER_HPP

#include "image_raw.hpp"

namespace pic {

//Border of a region
std::set<int> *SetBorder(ImageRAW *img, std::set<int> *coordsBorder)
{
    //Second border
    int ind, c, x, y;
    std::set<int> *ret = new std::set<int>;
    std::set<int>::iterator it;

    for(it = coordsBorder->begin(); it != coordsBorder->end(); it++) {
        ind = *it;
        img->ReverseAddress(ind, x, y);

        c = img->Address(x + 1, y);

        if(img->data[c] > 1.0f &&
           coordsBorder->find(c) == coordsBorder->end()) {
            ret->insert(c);
        }

        c = img->Address(x - 1, y);

        if(img->data[c] > 1.0f &&
           coordsBorder->find(c) == coordsBorder->end()) {
            ret->insert(c);
        }

        c = img->Address(x, y + 1);

        if(img->data[c] > 1.0f &&
           coordsBorder->find(c) == coordsBorder->end()) {
            ret->insert(c);
        }

        c = img->Address(x, y - 1);

        if(img->data[c] > 1.0f &&
           coordsBorder->find(c) == coordsBorder->end()) {
            ret->insert(c);
        }
    }

    return ret;
}

//Large border of a region
std::set<int> *SetBorderNth(ImageRAW *img, std::set<int> *coordsBorder,
                            int widthBorder)
{
    std::set<int> *ret = new std::set<int>;
    //Insert initial border
    ret->insert(coordsBorder->begin(), coordsBorder->end());

    for(int i = 0; i < widthBorder; i++) {
        std::set<int> *tmpBorder = SetBorder(img, ret);
        ret->insert(tmpBorder->begin(), tmpBorder->end());
    }

    return ret;
}

} // end namespace pic

#endif /* PIC_ALGORITHMS_REGION_BORDER_HPP */

