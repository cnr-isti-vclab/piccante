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

#ifndef PIC_UTIL_TILE_HPP
#define PIC_UTIL_TILE_HPP

#include "image.hpp"
#include "util/bbox.hpp"

namespace pic {

//Tile
class Tile
{
public:
    int				startX, startY;
    int				width,  height;
    std::string     name;
    Image		    *tile;

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
};

} // end namespace pic

#endif /* PIC_UTIL_TILE_HPP */

