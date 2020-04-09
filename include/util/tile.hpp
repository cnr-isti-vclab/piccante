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

#ifndef PIC_UTIL_TILE_HPP
#define PIC_UTIL_TILE_HPP

#include <string>

#include "../base.hpp"

#include "../image.hpp"
#include "../util/bbox.hpp"

namespace pic {

/**
 * @brief The Tile class
 */
class Tile
{
public:
    int startX, startY;
    int width,  height;
    std::string name;
    Image *tile;

    /**
     * @brief Tile
     */
    Tile()
    {
        startX = -1;
        startY = -1;
        width  = -1;
        height = -1;
        name = "";
        tile = NULL;
    }

    ~Tile()
    {
        if(tile != NULL) {
            delete tile;
        }

        tile = NULL;
    }

    /**
     * @brief getBBox
     * @param img_width
     * @param img_height
     * @return
     */
    BBox getBBox(int img_width, int img_height)
    {
        BBox ret;
        ret.setBox(startX,
                   startX + width,
                   startY,
                   startY + height,
                    0, 1, img_width, img_height, 1);
        return ret;
    }
};

} // end namespace pic

#endif /* PIC_UTIL_TILE_HPP */

