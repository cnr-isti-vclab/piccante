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

#ifndef PIC_UTIL_CACHED_TABLE_HPP
#define PIC_UTIL_CACHED_TABLE_HPP

namespace pic {

class CachedTable
{
public:
    int		n;
    float	COS_TABLE[512];
    float	SIN_TABLE[512];
    float	*PATCH;
    float	inv_width, inv_height;

    CachedTable(int patchSize, int width, int height)
    {
        n = 512;
        float C_PI_2_inv_n = C_PI_2 / float(n);

        for(int i = 0; i < n; i++) {
            float value = float(i) * C_PI_2_inv_n;

            COS_TABLE[i] = cosf(value);
            SIN_TABLE[i] = sinf(value);
        }

        PATCH = new float [patchSize + 1];
        int halfPatchSize = patchSize >> 1;

        for(int i = -halfPatchSize; i <= halfPatchSize; i++) {
            PATCH[i + halfPatchSize] = float(i);
        }

        inv_width  = 1.0f / float(width - 1);
        inv_height = 1.0f / float(height - 1);

    }
};

} // end namespace pic

#endif /* PIC_UTIL_CACHED_TABLE_HPP */

