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

#ifndef PIC_COLORS_COLOR_HPP
#define PIC_COLORS_COLOR_HPP

#include "../util/math.hpp"
#include "../util/vec.hpp"

namespace pic {

/**
 * @brief The Color class
 */

template <unsigned int N> class Color: public Vec<N, float>
{
public:

    //   R G B
    //   0 1 2

    Color<N>()
    {
        for(unsigned int i = 0; i < N; i++) {
            this->data[i] = 0.0f;
        }
    }

    /**
     * @brief Color
     * @param x
     * @param y
     * @param z
     */
    Color<N>(float x, float y, float z)
    {
        assert(N >= 3);

        this->data[0] = x;
        this->data[1] = y;
        this->data[2] = z;
    }

    /**
     * @brief Color
     * @param val
     */
    Color<N>(float *val)
    {
        assert(N >= 3);

        this->data[0] = val[0];
        this->data[1] = val[1];
        this->data[2] = val[2];
    }

    /**
     * @brief setBlack
     * @return
     */
    void setBlack()
    {
        for (unsigned int i = 0; i < N; i++) {
            this->data[i] = 0.0f;
        }
    }

    /**
    * @brief SetWhite
    */
    void setWhite()
    {
        for (unsigned int i = 0; i < N; i++) {
            this->data[i] = 1.0f;
        }
    }

    /**
     * @brief scaleTau
     * @param tau
     */
    void scaleTau(const Color<N> &tau)
    {
        for (int i = 0; i < N; i++) {
            this->data[i] *= expf(-tau.data[i]);
        }
    }

    /**
     * @brief scaleTau
     * @param sigma_t
     * @param tau
     */
    void scaleTau(const Color<N> &sigma_t, const Color<N> &tau)
    {
        for (int i = 0; i < N; i++) {
            this->data[i] *= expf(-tau.data[i] * sigma_t.data[i]);
        }
    }

    /**
     * @brief scaleTau
     * @param sigma_t
     * @param t
     */
    void scaleTau(const Color<N> &sigma_t, float t)
    {
        for (int i = 0; i < N; i++) {
            this->data[i] *= expf(-sigma_t.data[i] * t);
        }
    }

    /**
     * @brief convCRGB2XYZ
     */
    void convCRGB2XYZ()
    {
    }

    /**
     * @brief convCXYZ2RGB
     */
    void convCXYZ2RGB()
    {
    }

    /**
     * @brief Mean
     * @return
     */
    float getMean()
    {
        float ret = 0.0f;
        for (int i = 0; i < N; i++) {
            ret += this->data[i];
        }

        return ret / float(N);
    }

    /**
     * @brief luminance
     * @return
     */
    float luminance()
    {
        assert(N >= 3);

        return	0.213f * this->data[0] +
                0.715f * this->data[1] +
                0.072f * this->data[2];
    }

    /**
     * @brief saturate
     */
    void saturate()
    {
        for (int i = 0; i < N; i++) {
            this->data[i] = this->data[i] * 0.5f + 0.5f;
        }
    }

    /**
     * @brief isGreaterThanZero
     * @return
     */
    bool isGreaterThanZero()
    {
        bool ret = true;
        for (int i = 0; i < N; i++) {
            ret = ret && (this->data[i] > 0.0f);
        }
    }

    /**
     * @brief importanceSampling computes wavelength importance sampling: e is a uniform distributed random number in [0,1]
     * @param e
     * @param channel
     * @param pdf
     */
    void importanceSampling(float e, int &channel, float &pdf)
    {
        float sum = 0.0f;
        for(unsigned int i = 0; i < N; i++) {
            sum += this->data[i];
        }

        if(sum > 0.0f) {
            float CDF[N];
            CDF[0] = this->data[0] / sum;
            for(unsigned int i = 1; i < (N - 1); i++) {
                CDF[i] = (CDF[i - 1] + this->data[i]) / sum;
            }
            CDF[N - 1] = 1.0f; // sanity check

            for(unsigned int i = 0; i < N; i++) {
                if(e <= CDF[i]) {
                    channel = i;
                    pdf = this->data[i] / sum;
                }
            }
        } else {
            channel = int(e * float(N - 1));
            pdf = 1.0f / float(N);
        }
    }

    /**
     * @brief print
     */
    void print()
    {
        printf("\n Values :");
        for(unsigned int i = 0; i < N; i++) {
            printf("%d ", this->data[i]);
        }
        printf("\n");
    }

    /**
     * @brief gamma
     * @param g
     */
    void gamma(float g)
    {
        for (int i = 1; i < N; i++) {
            this->data[i] = pwof(this->data[i], g);
        }
    }

    /**
     * @brief inverse
     * @return
     */
    Color<N> inverse(float maxVal = -1.0f)
    {
        if(maxVal <= 0.0f) {
            maxVal = this->getMax();
        }

        Color<N> ret;

        for (int i = 0; i < N; i++) {
            ret.data[i] = maxVal - this->data[i];
        }

        return ret;
    }

    /**
     * @brief convertToArray
     * @param col
     */
    float *convertToArray(float *col)
    {
        if(col == NULL) {
            col = new float [N];
        }

        memcpy(col, this->data, N * sizeof(float));

        return col;
    }

    /**
    * @brief convertToLDR
    * @param exposure
    * @param gammaCor
    * @return
    */
    Color<N> convertToLDR(float exposure = 1.0f, float gammaCor = 2.2f)
    {
        Color<N> ret = this->clone();
        ret *= exposure;

        ret.gamma(1.0f / gammaCor);
        ret.clamp(0.0f, 1.0f);

        return ret;
    }

    /**
     * @brief Expf
     * @return
     */
    Color<N> exp()
    {
        Color<N> ret;

        return ret;
    }

    /**
     * @brief sqrt
     * @return
     */
    float sqrt()
    {
        for(unsigned int i = 0; i < N; i++) {
            this->data[i] = sqrtf(this->data[i]);
        }
    }

    /**
     * @brief TestImportanceSamplingColor executes a test Importance sampling of class Color.
     *
    static void TestImportanceSamplingColor()
    {
        Color col(1.0f, 0.8f, 0.3f);
        float p;
        int channel;
        printf("Testing Importance sampling for Color class:\n");

        for(int i = 0; i < 120; i++) {
            col.ImportanceSampling(float(rand() % 100) / 100.0f, channel, p);
            printf("[Channenl: %d;\t p: %f]\n", channel, p);
        }
    }*/
};

} // end namespace pic

#endif /* PIC_COLORS_COLOR_HPP */

