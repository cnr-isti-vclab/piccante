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

#ifndef PIC_FILTERING_FILTER_DRAGO_TMO_HPP
#define PIC_FILTERING_FILTER_DRAGO_TMO_HPP

#include "../util/array.hpp"
#include "../filtering/filter.hpp"
#include "../filtering/filter_luminance.hpp"

namespace pic {

/**
 * @brief The FilterDragoTMO class
 */
class FilterDragoTMO: public Filter
{
protected:
    float constant1, constant2, Lw_Max_scaled, Lw_a_scaled;
    float b, Ld_Max, Lw_Max, Lw_a;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box);

public:
    /**
     * @brief FilterDragoTMO
     */
    FilterDragoTMO();

    /**
     * @brief FilterDragoTMO
     * @param Ld_Max
     * @param b
     * @param Lw_Max
     * @param Lwa
     */
    FilterDragoTMO(float Ld_Max, float b, float Lw_Max, float Lwa);

    /**
     * @brief update
     * @param Ld_Max
     * @param b
     * @param Lw_Max
     * @param Lwa
     */
    void update(float Ld_Max, float b, float Lw_Max, float Lwa);
};

PIC_INLINE FilterDragoTMO::FilterDragoTMO() : Filter()
{
    update(100.0f, 0.95f, 1e6f, 0.5f);
}

PIC_INLINE FilterDragoTMO::FilterDragoTMO(float Ld_Max, float b, float Lw_Max,
                               float Lw_a) : Filter()
{
    update(Ld_Max, b, Lw_Max, Lw_a);
}

PIC_INLINE void FilterDragoTMO::update(float Ld_Max, float b, float Lw_Max,
                            float Lw_a)
{
    //protected values are assigned/computed
    this->Ld_Max = (Ld_Max > 0.0f) ? Ld_Max : 100.0f;
    this->b = (b > 0.0f) ? b : 0.95f;
    this->Lw_Max = (Lw_Max > 0.0f) ? Lw_Max : 1e6f;
    this->Lw_a = (Lw_a > 0.0f) ? Lw_a : 0.5f;

    //constants
    Lw_a_scaled   = this->Lw_a / powf(1.0f + b - 0.85f, 5.0f);
    Lw_Max_scaled = this->Lw_Max / Lw_a_scaled;

    constant1 = logf(b) / logf(0.5f);
    constant2 = (Ld_Max / 100.0f) / (log10f(1.0f + Lw_Max_scaled));
}

PIC_INLINE void FilterDragoTMO::ProcessBBox(Image *dst, ImageVec src, BBox *box)
{
    int channels = src[0]->channels;

    for(int j = box->y0; j < box->y1; j++) {

        for(int i = box->x0; i < box->x1; i++) {

            float *dataIn  = (*src[0])(i, j);
            float *dataLum = (*src[1])(i, j);
            float *dataOut = (*dst   )(i, j);

            if(dataLum[0] > 0.0f) {
                float L_scaled = dataLum[0] / Lw_a_scaled;

                float tmp = powf((L_scaled / Lw_Max_scaled), constant1);
                float Ld = constant2 * logf(1.0f + L_scaled) / logf(2.0f + 8.0f * tmp);

                for(int k = 0; k < channels; k++) {
                    dataOut[k] = (dataIn[k] * Ld) / dataLum[0];
                }
            } else {
                Array<float>::assign(0.0f, dataOut, src[0]->channels);
            }
        }
    }
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_DRAGO_TMO_HPP */

