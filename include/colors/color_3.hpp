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

#ifndef PIC_COLORS_COLOR_3_HPP
#define PIC_COLORS_COLOR_3_HPP

//typedef float float;
#include "util/math.hpp"

namespace pic {

/**
 * @brief The Color3 class
 */
class Color3
{
public:

    //   R G B
    //	 X Y Z
    //
    float x, y, z;

    Color3()
    {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
    }

    /**
     * @brief Color3
     * @param x
     * @param y
     * @param z
     */
    Color3(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    /**
     * @brief Color3
     * @param val
     */
    Color3(float *val)
    {
        this->x = val[0];
        this->y = val[1];
        this->z = val[2];
    }

    /**
     * @brief Black
     * @return
     */
    static Color3 Black()
    {
        Color3 ret;
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
        x = a;
        y = a;
        z = a;
    }

    /**
     * @brief operator +=
     * @param a
     */
    void operator +=(const float &a)
    {
        x += a;
        y += a;
        z += a;
    }

    /**
     * @brief operator +=
     * @param a
     */
    void operator +=(const float *a)
    {
        x += a[0];
        y += a[1];
        z += a[2];
    }

    /**
     * @brief operator +
     * @param a
     * @return
     */
    Color3 operator +(const float &a) const
    {
        return Color3(x + a, y + a, z + a);
    }

    /**
     * @brief operator -=
     * @param a
     */
    void operator -=(const float &a)
    {
        x -= a;
        y -= a;
        z -= a;
    }

    /**
     * @brief operator -
     * @param a
     * @return
     */
    Color3 operator -(const float &a) const
    {
        return Color3(x - a, y - a, z - a);
    }

    /**
     * @brief operator *=
     * @param a
     */
    void operator *=(const float &a)
    {
        x *= a;
        y *= a;
        z *= a;
    }

    /**
     * @brief operator *=
     * @param a
     */
    void operator *=(const float a[3])
    {
        x *= a[0];
        y *= a[1];
        z *= a[2];
    }

    /**
     * @brief operator *
     * @param a
     * @return
     */
    Color3 operator *(const float &a) const
    {
        return Color3(x * a, y * a, z * a);
    }

    /**
     * @brief operator /=
     * @param a
     */
    void operator /=(const float &a)
    {
        x /= a;
        y /= a;
        z /= a;
    }

    /**
     * @brief operator /
     * @param a
     * @return
     */
    Color3 operator /(const float &a) const
    {
        return Color3(x / a, y / a, z / a);
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
    void operator +=(const Color3 &col)
    {
        x += col.x;
        y += col.y;
        z += col.z;
    }

    /**
     * @brief operator +
     * @param col
     * @return
     */
    Color3 operator +(const Color3 &col) const
    {
        return Color3(x + col.x, y + col.y, z + col.z);
    }

    /**
     * @brief operator -=
     * @param col
     */
    void operator -=(const Color3 &col)
    {
        x -= col.x;
        y -= col.y;
        z -= col.z;
    }

    /**
     * @brief operator -
     * @return
     */
    Color3 operator -() const
    {
        return Color3(-x, -y, -z);
    }

    /**
     * @brief operator -
     * @param col
     * @return
     */
    Color3 operator -(const Color3 &col) const
    {
        return Color3(x - col.x, y - col.y, z - col.z);
    }

    /**
     * @brief operator *=
     * @param col
     */
    void operator *=(const Color3 &col)
    {
        x *= col.x;
        y *= col.y;
        z *= col.z;
    }

    /**
     * @brief operator *
     * @param col
     * @return
     */
    Color3 operator *(const Color3 &col)
    {
        return Color3(x * col.x, y * col.y, z * col.z);
    }

    /**
     * @brief operator /=
     * @param col
     */
    void operator /=(Color3 &col)
    {
        x /= col.x;
        y /= col.y;
        z /= col.z;
    }

    /**
     * @brief operator /
     * @param col
     * @return
     */
    Color3 operator /(Color3 &col) const
    {
        return Color3(x / col.x, y / col.y, z / col.z);
    }

    /**
     * @brief operator !=
     * @param col
     * @return
     */
    bool operator !=(Color3 &col)
    {
        return (col.x != x) || (col.y != y) || (col.z != z);
    }

    /**
     * @brief operator ==
     * @param col
     * @return
     */
    bool operator ==(Color3 &col)
    {
        return (col.x == x) && (col.y == y) && (col.z == z);
    }

    /**
     * @brief ScaleTau
     * @param tau
     */
    void ScaleTau(const Color3 *tau)
    {
        x *= exp(-tau->x);
        y *= exp(-tau->y);
        z *= exp(-tau->z);
    }

    /**
     * @brief ScaleTau
     * @param sigma_t
     * @param tau
     */
    void ScaleTau(const Color3 *sigma_t, const Color3 *tau)
    {
        x *= exp(-tau->x * sigma_t->x);
        y *= exp(-tau->y * sigma_t->y);
        z *= exp(-tau->z * sigma_t->z);
    }

    /**
     * @brief ScaleTau
     * @param sigma_t
     * @param t
     */
    void ScaleTau(const Color3 *sigma_t, float t)
    {
        x *= exp(-sigma_t->x * t);
        y *= exp(-sigma_t->y * t);
        z *= exp(-sigma_t->z * t);
    }

    /**
     * @brief SetBlack
     */
    void SetBlack()
    {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
    }

    /**
     * @brief SetWhite
     */
    void SetWhite()
    {
        x = 1.0f;
        y = 1.0f;
        z = 1.0f;
    }

    /**
     * @brief convCRGB2XYZ
     */
    void	convCRGB2XYZ()
    {
    }

    /**
     * @brief convCXYZ2RGB
     */
    void	convCXYZ2RGB()
    {
    }

    /**
     * @brief operator []
     * @param i
     * @return
     */
    float operator [](int i) const
    {
        switch(i % 3) {
        case 0:
            return x;
            break;

        case 1:
            return y;
            break;

        case 2:
            return z;
            break;
        }

        return 0.0f;
    }

    /**
     * @brief operator []
     * @param i
     * @return
     */
    float &operator [](int i)
    {
        switch(i % 3) {
        case 0:
            return x;
            break;

        case 1:
            return y;
            break;

        case 2:
            return z;
            break;
        }

        return x;
    }

    /**
     * @brief Mean
     * @return
     */
    float	Mean()
    {
        return (x + y + z) / 3.0f;
    }

    /**
     * @brief luminance
     * @return
     */
    float	luminance()
    {
        return	0.213f * x +
                0.715f * y +
                0.072f * z;
    }

    /**
     * @brief Saturate
     */
    void Saturate()
    {
        x = x * 0.5f + 0.5f;
        y = y * 0.5f + 0.5f;
        z = z * 0.5f + 0.5f;
    }

    /**
     * @brief scan
     * @param file
     */
    void scan(FILE *file)
    {
        fscanf(file, "%f", &x);
        fscanf(file, "%f", &y);
        fscanf(file, "%f", &z);
    }

    /**
     * @brief IsGreater0
     * @return
     */
    bool IsGreater0()
    {
        return (x > 0.0f) && (y > 0.0f) && (z > 0.0f);
    }

    /**
     * @brief GetMax
     * @return
     */
    float GetMax()
    {
        float tmp = x > y ? x : y;
        return tmp > z ? tmp : z;
    }

    /**
     * @brief GetMaxChannel
     * @return
     */
    int GetMaxChannel()
    {
        float valMax = GetMax();

        if(valMax == x) {
            return 0;
        }

        if(valMax == y) {
            return 1;
        }

        if(valMax == z) {
            return 2;
        }

        return -1;
    }

    /**
     * @brief ImportanceSampling computes wavelength importance sampling: e is a uniform distributed random number in [0,1]
     * @param e
     * @param channel
     * @param pdf
     */
    void ImportanceSampling(float e, int &channel, float &pdf)
    {
        float sum = x + y + z;

        if(sum > 0.0f) {
            float CDF[3];
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
        } else {
            channel = int(e * 2.0f);
            pdf = 1.0f / 3.0f;
        }
    }

    /**
     * @brief print
     */
    void print()
    {
        printf("\n Values --> R: %f G: %f B: %f\n", x, y, z);
    }

    /**
     * @brief gamma
     * @param g
     */
    void gamma(float g)
    {
        x = powf(x, g);
        y = powf(y, g);
        z = powf(z, g);

    }

    /**
     * @brief inverse
     * @return
     */
    Color3 inverse()
    {
        Color3 ret;
        float maxVal = MAX(MAX(x, y), z);
        ret.x = maxVal - x;
        ret.y = maxVal - y;
        ret.z = maxVal - z;

        return ret;
    }

    /**
     * @brief convertToArray
     * @param col
     */
    void convertToArray(float *col)
    {
        col[0] = x;
        col[1] = y;
        col[2] = z;
    }

    /**
     * @brief convertToArray
     * @return
     */
    float *convertToArray()
    {
        float *col = new float[3];

        col[0] = x;
        col[1] = y;
        col[2] = z;

        return col;
    }

    /**
     * @brief convertToLDR
     * @param exposure
     * @param gammaCor
     * @return
     */
    Color3 convertToLDR(float exposure = 1.0f, float gammaCor = 2.2f)
    {
        Color3 ret(x * exposure, y * exposure, z * exposure);
        ret.gamma(1.0f / gammaCor);

        ret.x = CLAMPi(ret.x, 0.0f, 1.0f);
        ret.y = CLAMPi(ret.y, 0.0f, 1.0f);
        ret.z = CLAMPi(ret.z, 0.0f, 1.0f);

        return ret;
    }

    /**
     * @brief Expf
     * @return
     */
    Color3 Expf()
    {
        Color3 col(expf(x), expf(y), expf(z));
        return col;
    }

    /**
     * @brief TestImportanceSamplingColor3 executes a test Importance sampling of class Color3.
     */
    static void TestImportanceSamplingColor3()
    {
        Color3 col(1.0f, 0.8f, 0.3f);
        float p;
        int channel;
        printf("Testing Importance sampling for Color3 class:\n");

        for(int i = 0; i < 120; i++) {
            col.ImportanceSampling(float(rand() % 100) / 100.0f, channel, p);
            printf("[Channenl: %d;\t p: %f]\n", channel, p);
        }
    }
};

//Colors
const Color3 RED    = Color3(1.0f, 0.0f, 0.0f);
const Color3 GREEN  = Color3(0.0f, 1.0f, 0.0f);
const Color3 BLUE   = Color3(0.0f, 0.0f, 1.0f);

const Color3 BLACK  = Color3(0.0f, 0.0f, 0.0f);
const Color3 WHITE  = Color3(1.0f, 1.0f, 1.0f);

} // end namespace pic

#endif /* PIC_COLORS_COLOR_3_HPP */

