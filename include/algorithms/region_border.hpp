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

#include <set>

#include "../base.hpp"
#include "../image.hpp"

namespace pic {

/**
 * @brief setBorder
 * @param img
 * @param coordsBorder
 * @return
 */
PIC_INLINE std::set<int> *setBorder(Image *img, std::set<int> *coordsBorder)
{
    //second border
    int ind, c, x, y;
    std::set<int> *ret = new std::set<int>;
    std::set<int>::iterator it;

    for(it = coordsBorder->begin(); it != coordsBorder->end(); it++) {
        ind = *it;
        img->reverseAddress(ind, x, y);

        c = img->getAddress(x + 1, y);

        if(img->data[c] > 1.0f &&
           coordsBorder->find(c) == coordsBorder->end()) {
            ret->insert(c);
        }

        c = img->getAddress(x - 1, y);

        if(img->data[c] > 1.0f &&
           coordsBorder->find(c) == coordsBorder->end()) {
            ret->insert(c);
        }

        c = img->getAddress(x, y + 1);

        if(img->data[c] > 1.0f &&
           coordsBorder->find(c) == coordsBorder->end()) {
            ret->insert(c);
        }

        c = img->getAddress(x, y - 1);

        if(img->data[c] > 1.0f &&
           coordsBorder->find(c) == coordsBorder->end()) {
            ret->insert(c);
        }
    }

    return ret;
}

/**
 * @brief setBorderNth
 * @param img
 * @param coordsBorder
 * @param widthBorder
 * @return
 */
PIC_INLINE std::set<int> *setBorderNth(Image *img, std::set<int> *coordsBorder,
                            int widthBorder)
{
    std::set<int> *ret = new std::set<int>;

    //insert initial border
    ret->insert(coordsBorder->begin(), coordsBorder->end());

    for(int i = 0; i < widthBorder; i++) {
        std::set<int> *tmpBorder = setBorder(img, ret);
        ret->insert(tmpBorder->begin(), tmpBorder->end());
    }

    return ret;
}

} // end namespace pic

#endif /* PIC_ALGORITHMS_REGION_BORDER_HPP */

