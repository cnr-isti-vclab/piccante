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

#ifndef PIC_UTIL_INDEXED_ARRAY_HPP
#define PIC_UTIL_INDEXED_ARRAY_HPP

namespace pic {

/**
 * @brief IntCoord
 */
typedef std::vector<int> IntCoord;

/**
 * @brief The IndexedArray class
 */
class IndexedArray
{
public:
    IndexedArray()
    {
    }

    /**
     * @brief bFuncNotNeg
     * @param val
     * @return
     */
    static bool bFuncNotNeg(float val)
    {
        return (val > 0.0f);
    }

    /**
     * @brief bFuncNotZero
     * @param val
     * @return
     */
    static bool bFuncNotZero(float val)
    {
        return (val < 0.0f) || (val > 0.0f);
    }

    /**
     * @brief bFuncNeg
     * @param val
     * @return
     */
    static bool bFuncNeg(float val)
    {
        return (val < 0.0f);
    }

    /**
     * @brief findSimple collects coordinates of data which satisfies a bool function func.
     * @param data
     * @param nData
     * @param stride
     * @return
     */
    static IntCoord findSimple(float *data, int nData, bool(*func)(float), int stride = 1)
    {
        IntCoord ret;

        for(int i = 0; i < nData; i += stride) {
            if(func(data[i])) {
                ret.push_back(i);
            }
        }

        return ret;
    }

    /**
     * @brief find collects coordinates of data which satisfies a bool function func.
     * @param data
     * @param nData
     * @param param
     * @return
     */
    static std::vector<int> find(float *data, int nData, bool(*func)(float,
                          std::vector<float>), std::vector<float> param)
    {

        std::vector<int> ret;

        for(int i = 0; i < nData; i++) {
            if(func(data[i], param)) {
                ret.push_back(i);
            }
        }

        return ret;
    }

    /**
     * @brief mean computes the mean value.
     * @param data
     * @param coord
     * @return
     */
    static float mean(float *data, IntCoord &coord)
    {
        float ret = 0.0f;

        for(unsigned int i = 0; i < coord.size(); i++) {
            ret += data[coord[i]];
        }

        return ret / float(coord.size());
    }

    /**
     * @brief max computes the max value.
     * @param data
     * @param coord
     * @return
     */
    static float max(float *data, IntCoord &coord)
    {
        float ret = -FLT_MAX;
        int tmp;

        for(unsigned int i = 0; i < coord.size(); i++) {
            tmp = coord[i];
            ret = ret < data[tmp] ? data[tmp] : ret;
        }

        return ret;
    }

    /**
     * @brief minCoord computes the min value.
     * @param data
     * @param coord
     * @return
     */
    static float min(float *data, IntCoord &coord)
    {
        float ret = FLT_MAX;
        int tmp;

        for(unsigned int i = 0; i < coord.size(); i++) {
            tmp = coord[i];
            ret = ret > data[tmp] ? data[tmp] : ret;
        }

        return ret;
    }

    /**
     * @brief scalingAddress scales values.
     * @param coord
     * @param scaling
     */
    static void scalingAddress(IntCoord &coord, int scaling)
    {
        for(unsigned int i = 0; i < coord.size(); i++) {
            coord.at(i) = coord.at(i) * scaling;
        }
    }

    /**
     * @brief log10Mean computes mean in the log10 domain.
     * @param data
     * @param coord
     * @return
     */
    static float log10Mean(float *data, IntCoord &coord)
    {
        float delta = 1e-6f;
        float ret = 0.0f;

        for(unsigned int i = 0; i < coord.size(); i++) {
            int tmp = coord[i];
            ret += log10f(data[tmp] + delta);
        }

        return ret / float(coord.size());
    }

    /**
     * @brief log2Mean computes mean in the log2 domain.
     * @param data
     * @param coord
     * @return
     */
    static float log2Mean(float *data, IntCoord &coord)
    {
        float delta = 1e-6f;
        float ret = 0.0f;
        float log2f = logf(2.0f);

        for(unsigned int i = 0; i < coord.size(); i++) {
            int tmp = coord[i];
            ret += logf(data[tmp] + delta) / log2f;
        }

        return ret / float(coord.size());
    }

    /**
     * @brief negative computes the negative value given a val reference point.
     * @param data
     * @param coord
     * @param referencePoint
     */
    static void negative(float *data, IntCoord &coord, float referencePoint = 1.0f)
    {
        for(unsigned int i = 0; i < coord.size(); i++) {
            int tmp = coord[i];
            data[tmp] = referencePoint - data[tmp];
        }
    }

    /**
     * @brief add is the additive operator.
     * @param data
     * @param coord
     * @param val
     */
    static void Add(float *data, IntCoord &coord, float val)
    {
        for(unsigned int i = 0; i < coord.size(); i++) {
            int tmp = coord[i];
            data[tmp] += val;
        }
    }

    /**
     * @brief sub is the subtractive operator.
     * @param data
     * @param coord
     * @param val
     */
    static void sub(float *data, IntCoord &coord, float val)
    {
        for(unsigned int i = 0; i < coord.size(); i++) {
            int tmp = coord[i];
            data[tmp] -= val;
        }
    }

    /**
     * @brief mul is the multiplicative operator.
     * @param data
     * @param coord
     * @param val
     */
    static void Mul(float *data, IntCoord &coord, float val)
    {
        for(unsigned int i = 0; i < coord.size(); i++) {
            int tmp = coord[i];
            data[tmp] *= val;
        }
    }

    /**
     * @brief div is the division operator.
     * @param data
     * @param coord
     * @param val
     */
    static void div(float *data, IntCoord &coord, float val)
    {
        for(unsigned int i = 0; i < coord.size(); i++) {
            int tmp = coord[i];
            data[tmp] /= val;
        }
    }

    /**
     * @brief assign
     * @param dataDst
     * @param coord
     * @param val
     */
    static void assign(float *dataDst, float dataSrc, IntCoord &coord)
    {
        for(unsigned int i = 0; i < coord.size(); i++) {
            int tmp = coord[i];
            dataDst[tmp] *= dataSrc;
        }
    }

    /**
     * @brief Assign
     * @param dataDst
     * @param dataSrc
     * @param coord
     */
    static void assign(float *dataDst, float *dataSrc, IntCoord &coord)
    {
        for(unsigned int i = 0; i < coord.size(); i++) {
            int tmp = coord[i];
            dataDst[tmp] = dataSrc[tmp];
        }
    }
};

} // end namespace pic

#endif /* PIC_UTIL_INDEXED_ARRAY_HPP */

