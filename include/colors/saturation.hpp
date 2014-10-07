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

#ifndef PIC_COLORS_SATURATION_HPP
#define PIC_COLORS_SATURATION_HPP

namespace pic {
/**
 * @brief computeSaturation
 * @param data
 * @param channels
 * @return
 */
inline float computeSaturation(float *data, int channels = 3)
{
    if(channels == 1) {
        return 1.0f;
    }

    float tmp, var, tmpMu;

    tmpMu = 0.0f;

    for(int i = 0; i < channels; i++) {
        tmpMu += data[i];
    }

    tmpMu /= float(channels);

    var = 0.0f;

    for(int i = 0; i < channels; i++) {
        tmp = data[i] - tmpMu;
        var += tmp * tmp;
    }

    var = sqrtf(var / float(channels));

    return var;
}


} // end namespace pic

#endif /* PIC_COLORS_SATURATION_HPP */
