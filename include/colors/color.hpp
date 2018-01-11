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

//typedef float float;
#include "util/math.hpp"

namespace pic {

/**
 * @brief The Color class
 */

template <unsigned int N> class Color
{
public:

    //   R G B
    //	 X Y Z
    //   0 1 2
    float data[N];

    Color()
    {
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

        data[0] = x;
        data[1] = y;
        data[2] = z;
    }

    /**
     * @brief Color
     * @param val
     */
    Color<N>(float *val)
    {
        assert(N >= 3);

        data[0] = val[0];
        data[1] = val[1];
        data[2] = val[2];
    }

    /**
     * @brief setBlack
     * @return
     */
    void setBlack()
    {
        for (unsigned int i = 0; i < N; i++) {
            data[i] = 0.0f;
        }
    }

    /**
    * @brief SetWhite
    */
    void setWhite()
    {
        for (unsigned int i = 0; i < N; i++) {
            data[i] = 1.0f;
        }
    }

    Color<N> clone()
    {
        Color<N> ret;
        memcpy(ret.data, this->data, N * sizeof(float));
        return ret;
    }

    /*
    *
    *
    */

    /*
    *
    *	Scalar Operands
    *
    */

    /**
     * @brief operator =
     * @param a
     */
    void operator =(const float &a)
    {
        for (int i = 0; i < N; i++) {
            data[i] = a;
        }
    }

    /**
     * @brief operator +=
     * @param a
     */
    void operator +=(const float &a)
    {
        for (int i = 0; i < N; i++) {
            data[i] += a;
        }
    }

    /**
     * @brief operator +=
     * @param a
     */
    void operator +=(const float *a)
    {
        for (int i = 0; i < N; i++) {
            data[i] = a[i];
        }
    }

    /**
     * @brief operator +
     * @param a
     * @return
     */
    Color<N> operator +(const float &a) const
    {
        Color<N> ret = this->clone();
        ret += a;
        return ret;
    }

    /**
     * @brief operator -=
     * @param a
     */
    void operator -=(const float &a)
    {
        for (int i = 0; i < N; i++) {
            data[i] -= a;
        }
    }

    /**
     * @brief operator -
     * @param a
     * @return
     */
    Color<N> operator -(const float &a) const
    {
        Color<N> ret = this->clone();
        ret -= a;
        return ret;
    }

    /**
     * @brief operator *=
     * @param a
     */
    void operator *=(const float &a)
    {
        for (int i = 0; i < N; i++) {
            data[i] *= a;
        }
    }

    /**
     * @brief operator *=
     * @param a
     */
    void operator *=(const float *a)
    {
        for (int i = 0; i < N; i++) {
            data[i] *= a[i];
        }
    }

    /**
     * @brief operator *
     * @param a
     * @return
     */
    Color<N> operator *(const float &a) const
    {
        Color<N> ret = this->clone();
        ret *= a;
        return ret;
    }

    /**
     * @brief operator /=
     * @param a
     */
    void operator /=(const float &a)
    {
        for (int i = 0; i < N; i++) {
            data[i] /= a;
        }
    }

    /**
     * @brief operator /
     * @param a
     * @return
     */
    Color<N> operator /(const float &a) const
    {
        Color<N> ret = this->clone();
        ret /= a;
        return ret;
    }

    /*
    *
    *	Color Operands
    *
    */

    /**
     * @brief operator +=
     * @param col
     */
    void operator +=(const Color<N> &col)
    {
        for (int i = 0; i < N; i++) {
            data[i] += col.data[i];
        }
    }

    /**
     * @brief operator +
     * @param col
     * @return
     */
    Color operator +(const Color<N> &col) const
    {
        Color<N> ret = this->clone();
        ret += col;
        return ret;
    }

    /**
     * @brief operator -=
     * @param col
     */
    void operator -=(const Color<N> &col)
    {
        for (int i = 0; i < N; i++) {
            data[i] -= col.data[i];
        }
    }

    /**
     * @brief operator -
     * @return
     */
    Color<N> operator -() const
    {
        Color<N> ret;
        for (int i = 0; i < N; i++) {
            ret.data[i] = -data[i];
        }

        return ret;
    }

    /**
     * @brief operator -
     * @param col
     * @return
     */
    Color operator -(const Color<N> &col) const
    {
        Color<N> ret = this->clone();
        ret -= col;
        return ret;
    }

    /**
     * @brief operator *=
     * @param col
     */
    void operator *=(const Color<N> &col)
    {
        for (int i = 0; i < N; i++) {
            data[i] *= col.data[i];
        }
    }

    /**
     * @brief operator *
     * @param col
     * @return
     */
    Color<N> operator *(const Color<N> &col)
    {
        Color<N> ret = this->clone();
        ret *= col;
        return ret;
    }

    /**
     * @brief operator /=
     * @param col
     */
    void operator /=(Color<N> &col)
    {
        for (int i = 0; i < N; i++) {
            data[i] /= col.data[i];
        }
    }

    /**
     * @brief operator /
     * @param col
     * @return
     */
    Color<N> operator /(Color &col) const
    {
        Color<N> ret = this->clone();
        ret /= col;
        return ret;
    }

    /**
     * @brief operator !=
     * @param col
     * @return
     */
    bool operator !=(Color<N> &col)
    {
        bool ret = false;
        for (int i = 0; i < N; i++) {
            ret = ret || (data[i] != col.data[i]);
        }
        return ret;
    }

    /**
     * @brief operator ==
     * @param col
     * @return
     */
    bool operator ==(Color<N> &col)
    {
        bool ret = true;
        for (int i = 0; i < N; i++) {
            ret = ret && (data[i] == col.data[i]);
        }
        return ret;
    }

    /**
     * @brief scaleTau
     * @param tau
     */
    void scaleTau(const Color<N> &tau)
    {
        for (int i = 0; i < N; i++) {
            data[i] *= expf(-tau.data[i]);
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
            data[i] *= expf(-tau.data[i] * sigma_t.data[i]);
        }
    }

    /**
     * @brief ScaleTau
     * @param sigma_t
     * @param t
     */
    void ScaleTau(const Color<N> &sigma_t, float t)
    {
        for (int i = 0; i < N; i++) {
            data[i] *= expf(-sigma_t.data[i] * t);
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
     * @brief operator []
     * @param i
     * @return
     */
    float operator [](int i) const
    {
        return data[i];
    }

    /**
     * @brief operator []
     * @param i
     * @return
     */
    float &operator [](int i)
    {
        return data[i];
    }

    /**
     * @brief Mean
     * @return
     */
    float getMean()
    {
        float ret = 0.0f;
        for (int i = 0; i < N; i++) {
            ret += data[i];
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

        return	0.213f * data[0] +
                0.715f * data[1] +
                0.072f * data[2];
    }

    /**
     * @brief saturate
     */
    void saturate()
    {
        for (int i = 0; i < N; i++) {
            data[i] = data[i] * 0.5f + 0.5f;
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
            ret = ret && (data[i] > 0.0f);
        }
    }

    /**
     * @brief getMax
     * @return
     */
    float getMax()
    {
        float ret = data[0];
        for (int i = 1; i < N; i++) {
            ret = data[i] > ret ? data[i] : ret;
        }
        return ret;
    }

    /**
     * @brief getMaxChannel
     * @return
     */
    int getMaxChannel()
    {
        float valMax = getMax();

        for (int i = 1; i < N; i++) {
            if (valMax == data[i]) {
                return i;
            }
        }

        return -1;
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
        for(unsigned int i=0; i<N; i++) {
            sum += data[i];
        }

        if(sum > 0.0f) {
            //TOBEFIXED
            /*
            float CDF[N];
            CDF[0] =  x   / sum;
            CDF[1] = (x + y) / sum;
            CDF[2] =  1.0f;

            if(e <= CDF[0]) {
                channel = 0;
                pdf = x / sum;
            } else {
                if(e <= CDF[1]) {
                    channel = 1;
                    pdf = y / sum;
                } else {
                    channel = 2;
                    pdf = z / sum;
                }
            }
            */
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
            printf("%d ", data[i]);
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
            data[i] = pwof(data[i], g);
        }
    }

    /**
     * @brief inverse
     * @return
     */
    Color<N> inverse()
    {
        float maxVal = getMax();

        Color<N> ret;

        for (int i = 0; i < N; i++) {
            ret.data[i] = maxVal - data[i];
        }

        return ret;
    }

    /**
     * @brief convertToArray
     * @param col
     */
    void convertToArray(float *col)
    {
        memcpy(data, col, N * sizeof(float));
    }

    /**
     * @brief convertToArray
     * @return
     */
    float *convertToArray()
    {
        float *col = new float[N];

        memcpy(col, data, N * sizeof(float));

        return col;
    }

    /**
    * @brief clamp
    * @param min
    * @param max
    * @return
    */
    void clamp(float min, float max)
    {
        for (int i = 0; i < N; i++) {
            data[i] = CLAMPi(data[i], min, max);
        }
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

