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

#ifndef PIC_FEATURES_MATCHING_POISSON_DESCRIPTOR_HPP
#define PIC_FEATURES_MATCHING_POISSON_DESCRIPTOR_HPP

#include <random>
#include "util/math.hpp"
#include "image.hpp"
#include "point_samplers/sampler_random.hpp"

namespace pic {

/**
 * @brief The PoissonDescriptor class
 */
class PoissonDescriptor
{
protected:
    int         	kernelSize;
    int             nSamples;
    unsigned int    subBlock;
    std::mt19937	*m;

    RandomSampler<2> *rs;

    /**
     * @brief GenerateSamples
     * @param kernelSize
     */
    void GenerateSamples(unsigned int kernelSize)
    {
        rs = new RandomSampler<2>(ST_BRIDSON, kernelSize, kernelSize, 1);
        rs->Write("../data/output/kernel.bmp", 0);
        nSamples = rs->samplesR.size() >> 1;
    }

    /**
     * @brief getAux computes a descriptor at position (x0,y0) with size n.
     * @param img
     * @param x0
     * @param y0
     * @param desc
     * @return
     */
    unsigned int *getAux(Image *img, int x0, int y0, unsigned int *desc = NULL)
    {
        unsigned int bits = sizeof(unsigned int) * 8;
        subBlock = (nSamples * (nSamples - 1)) / bits;
        if(desc == NULL) {
            desc = new unsigned int[subBlock];
            memset(desc, 0, sizeof(unsigned int) * subBlock);
        }

        int x[2], y[2];

        int c = 0;
        for(int i = 0; i < nSamples; i++) {
            x[0] = rs->samplesR[(i << 1)];
            x[1] = rs->samplesR[(i << 1) + 1];

            for(int j = 0; j < nSamples; j++) {
                if(i == j) {
                    continue;
                }

                y[0] = rs->samplesR[(j << 1)];
                y[1] = rs->samplesR[(j << 1) + 1];

                float *p_x_val = (*img)(x0 + x[0], y0 + x[1]);
                float *p_y_val = (*img)(x0 + y[0], y0 + y[1]);

                float p_x = 0.0f;
                float p_y = 0.0f;

                for(int k = 0; k < img->channels; k++) {
                    p_x	+= p_x_val[k];
                    p_y	+= p_y_val[k];
                }

                int p = c / bits;
                int shift = c % bits;

                unsigned int ret = (p_x < p_y) ? 1 : 0;
                desc[p] += (ret << shift);
                c++;

            }

        }

        return desc;
    }

public:

    /**
     * @brief PoissonDescriptor
     * @param kernelSize
     */
    PoissonDescriptor(int kernelSize = 16)
    {
        subBlock = 0;
        m = new std::mt19937(rand() % 10000);

        GenerateSamples(kernelSize);
    }

    ~PoissonDescriptor()
    {
        Release();
    }

    /**
     * @brief Release deallocates memory.
     */
    void Release()
    {

        if(m != NULL) {
            delete m;
            m = NULL;
        }
    }

    /**
     * @brief get computes a descriptor at position (x0,y0) with size n.
     * @param img
     * @param x0
     * @param y0
     * @param desc
     * @return
     */
    unsigned int *get(Image *img, int x0, int y0, unsigned int *desc = NULL)
    {
        if(img == NULL) {
            return NULL;
        }

        if(!img->checkCoordinates(x0, y0)) {
            return NULL;
        }

        return getAux(img, x0, y0, desc);
    }

    /**
     * @brief getDescriptorSize returns the descriptor size.
     * @return the descriptor size.
     */
    int getDescriptorSize() {
        return subBlock;
    }

};

} // end namespace pic

#endif /* PIC_FEATURES_MATCHING_BRIEF_DESCRIPTOR_HPP */

