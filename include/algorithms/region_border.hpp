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

#ifndef PIC_ALGORITHMS_REGION_BORDER_HPP
#define PIC_ALGORITHMS_REGION_BORDER_HPP

#include "image.hpp"

namespace pic {

/**
 * @brief SetBorder
 * @param img
 * @param coordsBorder
 * @return
 */
std::set<int> *SetBorder(Image *img, std::set<int> *coordsBorder)
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

/**
 * @brief SetBorderNth
 * @param img
 * @param coordsBorder
 * @param widthBorder
 * @return
 */
std::set<int> *SetBorderNth(Image *img, std::set<int> *coordsBorder,
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

