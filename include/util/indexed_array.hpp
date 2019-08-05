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

#ifndef PIC_UTIL_INDEXED_ARRAY_HPP
#define PIC_UTIL_INDEXED_ARRAY_HPP

#include <vector>

#include "../util/math.hpp"

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
        return val > 0.0f;
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
     * @param ret
     * @param stride
     */
    static void findSimple(float *data, int nData, bool(*func)(float), IntCoord &ret, int stride = 1)
    {
        for(int i = 0; i < nData; i += stride) {
            if(func(data[i])) {
                ret.push_back(i);
            }
        }
    }

    /**
     * @brief find collects coordinates of data which satisfies a bool function func.
     * @param data
     * @param nData
     * @param param
     * @param ret
     */
    static void find(float *data, int nData, bool(*func)(float,
                     std::vector<float>), std::vector<float> param, IntCoord &ret)
    {
        for(int i = 0; i < nData; i++) {
            if(func(data[i], param)) {
                ret.push_back(i);
            }
        }
    }

    /**
     * @brief mean computes the mean value.
     * @param data
     * @param coord
     * @return
     */
    static float mean(float *data, IntCoord &coord)
    {
        if(coord.empty()) {
            return FLT_MAX;
        }

        float ret = 0.0f;

        for(auto i = 0; i < coord.size(); i++) {
            ret += data[coord[i]];
        }

        return ret / float(coord.size());
    }

    /**
     * @brief min computes the min value.
     * @param data
     * @param coord
     * @return
     */
    static float min(float *data, IntCoord &coord)
    {
        if(coord.empty()) {
            return FLT_MAX;
        }

        float ret = data[coord[0]];

        for(unsigned int i = 1; i < coord.size(); i++) {
            int j = coord[i];
            ret = MIN(ret, data[j]);
        }

        return ret;
    }

    /**
     * @brief max computes the max value.
     * @param data
     * @param coord
     * @return
     */
    static float max(float *data, IntCoord &coord)
    {
        if(coord.empty()) {
            return -FLT_MAX;
        }

        float ret = data[coord[0]];

        for(unsigned int i = 1; i < coord.size(); i++) {
            int j = coord[i];
            ret = MAX(ret, data[j]);
        }

        return ret;
    }

    /**
     * @brief percentile
     * @param data
     * @param coord
     * @param percent
     * @return
     */
    static float percentile(float *data, IntCoord &coord, float percent)
    {
        if(coord.empty()) {
            return FLT_MAX;
        }

        auto n = coord.size();
        float *tmp = new float[n];

        for(unsigned int i = 0; i < n; i++) {
            int j = coord[i];
            tmp[i] = data[j];
        }

        std::sort(tmp, tmp + n);

        percent = CLAMPi(percent, 0.0f, 1.0f);

        float ret = tmp[int(float(n - 1) * percent)];
        delete[] tmp;

        return ret;
    }

    /**
     * @brief scale scales values.
     * @param coord
     * @param scaling
     */
    static void scale(IntCoord &coord, int scale)
    {
        for(unsigned int i = 0; i < coord.size(); i++) {
            coord.at(i) = coord.at(i) * scale;
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
        if(coord.empty()) {
            return FLT_MAX;
        }

        float delta = 1e-6f;
        float ret = 0.0f;

        for(unsigned int i = 0; i < coord.size(); i++) {
            int j = coord[i];
            ret += log10f(data[j] + delta);
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
        if(coord.empty()) {
            return FLT_MAX;
        }

        float delta = 1e-6f;
        float ret = 0.0f;
        float log2f = logf(2.0f);

        for(unsigned int i = 0; i < coord.size(); i++) {
            int j = coord[i];
            ret += logf(data[j] + delta) / log2f;
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
            int j = coord[i];
            data[j] = referencePoint - data[j];
        }
    }

    /**
     * @brief add is the additive operator.
     * @param data
     * @param coord
     * @param val
     */
    static void add(float *data, IntCoord &coord, float val)
    {
        for(unsigned int i = 0; i < coord.size(); i++) {
            int j = coord[i];
            data[j] += val;
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
            int j = coord[i];
            data[j] -= val;
        }
    }

    /**
     * @brief mul is the multiplicative operator.
     * @param data
     * @param coord
     * @param val
     */
    static void mul(float *data, IntCoord &coord, float val)
    {
        for(unsigned int i = 0; i < coord.size(); i++) {
            int j = coord[i];
            data[j] *= val;
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
            int j = coord[i];
            data[j] /= val;
        }
    }

    /**
     * @brief assign
     * @param dataDst
     * @param coord
     * @param val
     */
    static void assign(float *data, IntCoord &coord, float val)
    {
        for(unsigned int i = 0; i < coord.size(); i++) {
            int j = coord[i];
            data[j] = val;
        }
    }

    /**
     * @brief Assign
     * @param dataDst
     * @param coord
     * @param dataSrc
     */
    static void assign(float *dataDst, IntCoord &coord, float *dataSrc)
    {
        for(unsigned int i = 0; i < coord.size(); i++) {
            int j = coord[i];
            dataDst[j] = dataSrc[j];
        }
    }
};

} // end namespace pic

#endif /* PIC_UTIL_INDEXED_ARRAY_HPP */

