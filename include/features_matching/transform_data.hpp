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

#ifndef PIC_FEATURES_MATCHING_TRANSFORM_DATA_HPP
#define PIC_FEATURES_MATCHING_TRANSFORM_DATA_HPP

namespace pic {

/**
 * @brief The TransformData class
 */
class TransformData
{
public:
    int		x, y;
    float	angle, scale;
    float	gain, bias;

    /**
     * @brief TransformData
     */
    TransformData()
    {
    }

    /**
     * @brief TransformData
     * @param x
     * @param y
     */
    TransformData(int x, int y)
    {
        this->x = x;
        this->y = y;
    }

    /**
     * @brief TransformData
     * @param x
     * @param y
     * @param angle
     * @param scale
     */
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

