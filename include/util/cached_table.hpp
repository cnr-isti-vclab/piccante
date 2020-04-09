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

#ifndef PIC_UTIL_CACHED_TABLE_HPP
#define PIC_UTIL_CACHED_TABLE_HPP

namespace pic {

/**
 * @brief The CachedTable class
 */
class CachedTable
{
public:
    int n;
    float COS_TABLE[512];
    float SIN_TABLE[512];
    float *PATCH;
    float inv_width, inv_height;

    /**
     * @brief CachedTable creates a precomputed table of sin and cos values.
     * @param patchSize is the size of the patch.
     * @param width is the width of the image.
     * @param height is the height of the image.
     */
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

