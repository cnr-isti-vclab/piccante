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
#include <chrono>
#include <vector>

#include "../base.hpp"
#include "../util/std_util.hpp"
#include "../util/math.hpp"
#include "../image.hpp"

#ifndef PIC_DISABLE_EIGEN

#ifndef PIC_EIGEN_NOT_BUNDLED
    #include "../externals/Eigen/Dense"
#else
    #include <Eigen/Dense>
#endif

#endif

namespace pic {

#ifndef PIC_DISABLE_EIGEN

/**
 * @brief The BRIEFDescriptor class
 */
class BRIEFDescriptor
{
protected:
    int S;
    unsigned int n;
    float sigma_sq_2, sigma_sq;
    std::mt19937 *m;

    //samples coordinates
    int *x, *y;

    /**
     * @brief generateSample
     * @param sample
     */
    void generateSample(int *sample)
    {
        float theta = C_PI_2 * getRandom((*m)());

        float u = getRandom((*m)());
        float r = sqrtf(MAX(-logf(u) * sigma_sq_2, 0.0f));

        sample[0] = int(r * cosf(theta));
        sample[1] = int(r * sinf(theta));
    }

    /**
     * @brief generateSamples
     * @param n
     */
    void generateSamples(unsigned int n)
    {
        this->n = n;
        unsigned int n2 = n * 2;

        x = new int [n2];
        y = new int [n2];

        for(unsigned int i = 0; i < n2; i += 2) {
            generateSample(&x[i]);
            generateSample(&y[i]);
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
    uint* getAux(Image *img, int x0, int y0, int *x, int *y, uint *desc = NULL)
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
    BRIEFDescriptor(int S = 32, int n = 256, int seed = 1)
    {
        if(seed >= 0) {
            m = new std::mt19937(seed);
        } else {
            auto seed_time = std::chrono::system_clock::now().time_since_epoch().count();
            m = new std::mt19937(int (seed_time));
        }

        this->S = S;
        this->sigma_sq = float(S * S) / 25.0f;
        this->sigma_sq_2 = 2.0f * this->sigma_sq;

        generateSamples(n);
    }

    ~BRIEFDescriptor()
    {
        release();
    }

    /**
     * @brief Release deallocates memory.
     */
    void release()
    {
        m = delete_s(m);
        x = delete_s(x);
        y = delete_s(y);
    }

    /**
     * @brief get computes a descriptor at position (x0,y0) with size n.
     * @param img
     * @param x0
     * @param y0
     * @param desc
     * @return
     */
    uint *get(Image *img, int x0, int y0, uint *desc = NULL)
    {
        if(img == NULL) {
            return NULL;
        }

        if(!img->checkCoordinates(x0, y0)) {
            return NULL;
        }

        return getAux(img, x0, y0, x, y, desc);
    }

    #ifndef PIC_DISABLE_EIGEN
    /**
     * @brief getAll
     * @param descs
     * @param corners
     * @param img
     */
    void getAll(Image *img,
                std::vector< Eigen::Vector2f > &corners,
                std::vector< uint* > &descs)
    {
        descs.clear();

        for(unsigned int i = 0; i < corners.size(); i++) {
            int x0 = int(corners[i][0]);
            int y0 = int(corners[i][1]);
            descs.push_back(get(img, x0, y0, NULL));
        }
    }
    #endif

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
    static uint match(uint *fv0, uint *fv1, uint nfv)
    {
        if((fv0 == NULL) || (fv1 == NULL)) {
            return 0;
        }

        uint ret = 0;
        for(uint i = 0; i < nfv; i++) {
            ret += countZeros(fv0[i] ^ fv1[i]);
        }

        return ret;
    }
};

#endif

} // end namespace pic

#endif /* PIC_FEATURES_MATCHING_BRIEF_DESCRIPTOR_HPP */

