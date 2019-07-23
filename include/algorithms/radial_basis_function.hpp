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

#ifndef PIC_ALGORITHMS_RADIAL_BASIS_FUNCTION
#define PIC_ALGORITHMS_RADIAL_BASIS_FUNCTION

#include <math.h>

#include "../util/std_util.hpp"

namespace pic {

/**
 * @brief The RadialBasisFunction class
 */
class RadialBasisFunction
{
public:
    float *centers, var;
    int nDim, nCenters;

    /**
     * @brief RadialBasisFunction
     */
    RadialBasisFunction()
    {
        var = 0.0f;
        centers = NULL;
        nDim = 0;
        nCenters = 0;
    }

    ~RadialBasisFunction()
    {
        this->centers = delete_vec_s(this->centers);
    }

    /**
     * @brief update
     * @param centers
     * @param nCenters
     * @param nDim
     * @param var
     */
    void update(float *centers, int nCenters, int nDim, float var)
    {
        this->nDim = nDim;
        this->nCenters = nCenters;
        this->var = var;

        if(centers != NULL) {
            this->centers = delete_vec_s(this->centers);
            this->centers = new float[nCenters * nDim];
            memcpy(this->centers, centers, sizeof(float) * nCenters * nDim);
        }
    }

    /**
     * @brief eval
     * @param value
     * @return
     */
    float eval(float *value)
    {
        float ret = 0.0f;

        float sigma_sq_2 = var * 2.0f;
        for(int i = 0; i < nCenters; i++) {
            int index_i = i * nDim;

            float d_sq = 0.0f;
            for(int j = 0; j < nDim; j++) {
                int index_j = index_i + j;

                float d_j = (centers[index_j] - value[j]);
                d_sq += d_j * d_j;
            }

            ret += expf(-d_sq / sigma_sq_2);
        }

        return ret;
    }
};

} // end namespace pic

#endif /* PIC_ALGORITHMS_RADIAL_BASIS_FUNCTIONS */

