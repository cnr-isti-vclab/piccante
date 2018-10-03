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

#ifndef PIC_FILTERING_FILTER_SIMPLE_TMO_HPP
#define PIC_FILTERING_FILTER_SIMPLE_TMO_HPP

#include "../filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterSimpleTMO class
 */
class FilterSimpleTMO: public Filter
{
protected:
    float gamma, fstop, exposure;

    /**
     * @brief f
     * @param data
     */
    void f(FilterFData *data)
    {
        float *dataIn = (*data->src[0])(data->x, data->y);

        for(int k = 0; k < data->dst->channels; k++) {
            data->out[k] = powf((dataIn[k] * exposure), gamma);
        }
    }

public:
    /**
     * @brief FilterSimpleTMO
     * @param gamma
     * @param fstop
     */
    FilterSimpleTMO(float gamma, float fstop) : Filter()
    {
        update(gamma, fstop);
    }

    /**
     * @brief update
     * @param gamma
     * @param fstop
     */
    void update(float gamma, float fstop)
    {
        this->gamma = 1.0f / gamma;
        this->fstop = fstop;
        exposure = powf(2.0f, fstop);
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param gamma
     * @param fstop
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut, float gamma,
                             float fstop)
    {
        FilterSimpleTMO filter(gamma, fstop);
        return filter.ProcessP(Single(imgIn), imgOut);
    }

    /**
     * @brief execute
     * @param nameIn
     * @param nameOut
     * @param gamma
     * @param fstop
     * @return
     */
    static Image *execute(std::string nameIn, std::string nameOut, float gamma,
                             float fstop)
    {
        Image imgIn(nameIn);
        Image *imgOut = execute(&imgIn, NULL, gamma, fstop);
        imgOut->Write(nameOut, LT_NOR_GAMMA, 0);
        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_SIMPLE_TMO_HPP */

