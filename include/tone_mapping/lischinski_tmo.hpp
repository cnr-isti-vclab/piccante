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

#ifndef PIC_TONE_MAPPING_LISCHINSKI_TMO_HPP
#define PIC_TONE_MAPPING_LISCHINSKI_TMO_HPP

#include "../base.hpp"
#include "../util/math.hpp"
#include "../algorithms/lischinski_minimization.hpp"
#include "../tone_mapping/reinhard_tmo.hpp"
#include "../tone_mapping/tone_mapping_operator.hpp"

namespace pic {

class LischinskiTMO: public ToneMappingOperator
{
protected:
    FilterLuminance flt_lum;

    float alpha, whitePoint;

    /**
     * @brief ProcessAux
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *ProcessAux(ImageVec imgIn, Image *imgOut)
    {
        updateImage(imgIn[0]);

        //extract luminance
        images[0] = flt_lum.Process(imgIn, images[0]);

        float minL, maxL, Lav;

        images[0]->getMinVal(NULL, &minL);
        images[0]->getMaxVal(NULL, &maxL);
        images[0]->getLogMeanVal(NULL, &Lav);

        float minL_log = log2fPlusEpsilon(minL);
        float maxL_log = log2fPlusEpsilon(maxL);

        int Z = int(ceilf(maxL_log - minL_log));

        if(Z <= 0) {
            return imgOut;
        }

        if(alpha <= 0.0f) {
            alpha = ReinhardTMO::estimateAlpha(minL, maxL, Lav);
        }

        if(whitePoint <= 0.0f) {
            whitePoint = ReinhardTMO::estimateWhitePoint(minL, maxL);
        }

        float whitePoint_sq = whitePoint * whitePoint;

        //choose the representative Rz for each zone
        std::vector<float> *zones = new std::vector<float>[Z];
        float *fstop = new float[Z];
        float *Rz = new float[Z];

        Array<float>::assign(0.0f, Rz, Z);
        Array<float>::assign(0.0f, fstop, Z);

        for(int i = 0; i < images[0]->size(); i++) {
            float L = images[0]->data[i];
            float L_log = log2fPlusEpsilon(L);

            int zone = CLAMP(int(ceilf(L_log - minL_log)), Z);
            zones[zone].push_back(L);
        }

        for(int i = 0; i < Z; i++) {
            if(!zones[i].empty()) {
                std::sort(zones[i].begin(), zones[i].end());
                Rz[i] = zones[i][zones[i].size() >> 1];

                if(Rz[i] > 0.0f) {
                    float Rz_s = Rz[i] * alpha / Lav; //photographic operator
                    float f = (Rz_s * (1.0f + Rz_s / whitePoint_sq) ) / (1.0f + Rz_s);
                    fstop[i] = log2fPlusEpsilon(f / Rz[i]);
                }
            }
        }

        //create the fstop map
        images[0]->applyFunction(log2fPlusEpsilon);

        if(images[1] == NULL) {
            images[1] = images[0]->allocateSimilarOne();
        }

        for(int i = 0; i < images[0]->size(); i++) {
            float L_log = images[0]->data[i];
            int zone = CLAMP(int(ceilf(L_log - minL_log)), Z);
            images[1]->data[i] = fstop[zone];
        }

        //run Lischinski minimization
        images[2] = LischinskiMinimization(images[0], images[1], NULL, 0.007f, images[2]);

        images[2]->applyFunction(pow2f);

        *imgOut = *imgIn[0];
        *imgOut *= images[2];

        delete[] zones;
        delete[] Rz;
        delete[] fstop;

        return imgOut;
    }

public:

    /**
     * @brief LischinskiTMO
     * @param alpha
     * @param whitePoint
     */
    LischinskiTMO(float alpha = 0.15f, float whitePoint = 1e6f)
    {
        images.push_back(NULL);
        images.push_back(NULL);
        images.push_back(NULL);
        update(alpha, whitePoint);
    }

    /**
     * @brief update
     * @param alpha
     * @param whitePoint
     */
    void update(float alpha = 0.15f, float whitePoint = 1e6f)
    {
        this->alpha = alpha;
        this->whitePoint = whitePoint;
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut)
    {
        LischinskiTMO ltmo(0.15f, 1e6f);
        return ltmo.Process(Single(imgIn), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_TONE_MAPPING_LISCHINSKI_TMO_HPP */

