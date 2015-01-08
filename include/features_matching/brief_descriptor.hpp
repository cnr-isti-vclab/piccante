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

#ifndef PIC_FEATURES_MATCHING_BRIEF_DESCRIPTOR_HPP
#define PIC_FEATURES_MATCHING_BRIEF_DESCRIPTOR_HPP

#include <random>
#include "util/math.hpp"
#include "image.hpp"

namespace pic {

/**
 * @brief The BRIEFDescriptor class
 */
class BRIEFDescriptor
{
protected:
    int         	S;
    unsigned int    n;
    float   		sigma2_2, sigma2;
    std::mt19937	*m;

    int			*x, *y;

    /**
     * @brief GenerateSample
     * @param sample
     */
    void GenerateSample(int *sample)
    {
        float theta = C_PI_2 * Random((*m)());

        float u = Random((*m)());
        float r = sqrtf(MAX(-logf(u) * sigma2_2, 0.0f));

        sample[0] = int(r * cosf(theta));
        sample[1] = int(r * sinf(theta));
    }

    /**
     * @brief GenerateSamples
     * @param n
     */
    void GenerateSamples(unsigned int n)
    {
        this->n = n;
        unsigned int n2 = n * 2;

        x = new int [n2];
        y = new int [n2];

        for(unsigned int i = 0; i < n2; i += 2) {
            GenerateSample(&x[i]);
            GenerateSample(&y[i]);
        }
    }

    /**
     * @brief countZeros
     * @param x
     * @return
     */
    static unsigned int countZeros(unsigned int x)
    {
        unsigned int n = sizeof(unsigned int) * 8;

        unsigned int ret = 0;

        for(unsigned int i = 0; i < n; i++) {
            if((x & (1 << i)) == 0) {
                ret++;
            }
        }

        return ret;
    }

    /**
     * @brief getAux computes a descriptor at position (x0,y0) with size n.
     * @param img
     * @param x0
     * @param y0
     * @param desc
     * @return
     */
    unsigned int *getAux(Image *img, int x0, int y0, int *x, int *y, unsigned int *desc = NULL)
    {
        unsigned int bits = sizeof(unsigned int) * 8;
        unsigned int subBlock = n / bits;

        if(desc == NULL) {
            desc = new unsigned int[subBlock];
        }

        int c = 0;

        for(unsigned int i = 0; i < subBlock; i++) {
            unsigned int value = 0;

            for(unsigned int j = 0; j < bits; j++) {
                int cShifted = c * 2;

                float *p_x_val = (*img)(x0 + x[cShifted], y0 + x[cShifted + 1]);
                float *p_y_val = (*img)(x0 + y[cShifted], y0 + y[cShifted + 1]);

                float p_x = 0.0f;
                float p_y = 0.0f;

                for(int k = 0; k < img->channels; k++) {
                    p_x	+= p_x_val[k];
                    p_y	+= p_y_val[k];
                }

                unsigned int ret = (p_x < p_y) ? 1 : 0;

                value += (ret << j);

                c++;
            }

            desc[i] = value;
        }

        return desc;
    }

public:

    /**
     * @brief BRIEFDescriptor
     * @param S
     * @param n
     */
    BRIEFDescriptor(int S = 32, int n = 256)
    {
        m = new std::mt19937(rand() % 10000);

        this->S = S;
        this->sigma2 = float(S * S) / 25.0f;
        this->sigma2_2 = 2.0f * this->sigma2;

        GenerateSamples(n);
    }

    ~BRIEFDescriptor()
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

        if(x != NULL) {
            delete[] x;
            x = NULL;
        }

        if(y != NULL) {
            delete[] y;
            y = NULL;
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

        return getAux(img, x0, y0, x, y, desc);
    }

    /**
     * @brief getDescriptorSize returns the descriptor size.
     * @return the descriptor size.
     */
    int getDescriptorSize() {
        return n / (sizeof(unsigned int) * 8);
    }

    /**
     * @brief match matches two descriptors. Note: Higher scores means better matching.
     * @param fv0
     * @param fv1
     * @param nfv
     * @return
     */
    static unsigned int match(unsigned int *fv0, unsigned int *fv1, unsigned int nfv)
    {
        if((fv0 == NULL) && (fv1 == NULL)) {
            return 0;
        }

        unsigned int ret = 0;

        for(unsigned int i = 0; i < nfv; i++) {
            ret += countZeros(fv0[i] ^ fv1[i]);
        }

        return ret;
    }
};

} // end namespace pic

#endif /* PIC_FEATURES_MATCHING_BRIEF_DESCRIPTOR_HPP */

