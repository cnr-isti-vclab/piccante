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

#ifndef PIC_FEATURES_MATCHING_TRANSFORM_DATA_HPP
#define PIC_FEATURES_MATCHING_TRANSFORM_DATA_HPP

namespace pic {

class TransformData
{
public:
    int		x, y;
    float	angle, scale;
    float	gain, bias;

    TransformData()
    {
    }

    TransformData(int x, int y)
    {
        this->x = x;
        this->y = y;
    }

    TransformData(int x, int y, float angle, float scale)
    {
        this->x = x;
        this->y = y;
        this->angle = angle;
        this->scale = scale;
    }
};

} // end namespace pic

#endif /* PIC_FEATURES_MATCHING_TRANSFORM_DATA_HPP */

