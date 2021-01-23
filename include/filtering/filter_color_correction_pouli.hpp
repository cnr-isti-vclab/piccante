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

#ifndef PIC_FILTERING_FILTER_COLOR_CORRECTION_POULI_HPP
#define PIC_FILTERING_FILTER_COLOR_CORRECTION_POULI_HPP

#include "../filtering/filter.hpp"
#include "../colors/color_conv_rgb_to_lms.hpp"
#include "../colors/color_conv_lms_to_ipt.hpp"
#include "../colors/color_conv_ipt_to_ich.hpp"

namespace pic {

/**
 * @brief The FilterColorCorrectionPouli class
 */
class FilterColorCorrectionPouli: public Filter
{
protected:
    ColorConvRGBtoLMS cLMS;
    ColorConvLMStoIPT cIPT;
    ColorConvIPTtoICH cICH;
    float mHDR, mTMO;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box)
    {
        float ICh_hdr[3];
        float ICh_tmo[3];
        float nHDR[3], nTMO[3], tHDR[3], tTMO[3];
        for(int j = box->y0; j < box->y1; j++) {

            for(int i = box->x0; i < box->x1; i++) {

                float *data_hdr = (*src[0])(i, j);
                float *data_tmo = (*src[1])(i, j);

                //normalize inputs
                for(int k = 0; k < 3; k++) {
                    nHDR[k] = data_hdr[k] / mHDR;
                    nTMO[k] = data_tmo[k] / mTMO;
                }

                //RGB --> LMS
                cLMS.direct(nHDR, tHDR);
                cLMS.direct(nTMO, tTMO);

                //LMS --> IPT
                cIPT.direct(tHDR, nHDR);
                cIPT.direct(tTMO, nTMO);

                //IPT --> ICh
                cICH.direct(nHDR, ICh_hdr);
                cICH.direct(nTMO, ICh_tmo);

                float tmp = ICh_tmo[0];
                ICh_tmo[0] += 1e-5f;
                ICh_tmo[1] += 1e-5f;
                ICh_hdr[0] += 1e-5f;
                ICh_hdr[1] += 1e-5f;

                float C_tmo_prime = ICh_tmo[1] * ICh_hdr[0] / ICh_tmo[0];
                float s1 = saturation(ICh_hdr[1], ICh_hdr[0]);
                float s2 = saturation(C_tmo_prime, ICh_tmo[0]);

                ICh_tmo[0] = tmp;
                ICh_tmo[1] = (C_tmo_prime * s1) / s2;
                ICh_tmo[2] = ICh_hdr[2];

                //output
                float *data_dst = (*dst)(i, j);

                cICH.inverse(ICh_tmo, tTMO);
                cIPT.inverse(tTMO, nTMO);
                cLMS.inverse(nTMO, data_dst);

                Arrayf::mul(data_dst, 3, mTMO);
                Arrayf::clamp(data_dst, 3, 0.0f, 1.0f);
            }
        }
    }

public:

    /**
     * @brief FilterColorCorrectionPouli
     */
    FilterColorCorrectionPouli() : Filter()
    {
        minInputImages = 2;
    }

    void update(float mHDR, float mTMO)
    {
        this->mHDR = mHDR;
        this->mTMO = mTMO;
    }

    /**
     * @brief saturation
     * @param C
     * @param I
     * @return
     */
    static float saturation(float C, float I)
    {
        float sum = C * C + I * I;
        if(sum > 0.0f) {
            return C / sqrtf(sum);
        } else {
            return -1.0f;
        }
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param gamma
     * @param fstop
     * @return
     */
    static Image *execute(Image *imgHDR, Image *imgTMO, Image *imgOut)
    {
        if(imgHDR == NULL || imgTMO == NULL) {
            return imgOut;
        }

        if(imgHDR->channels != 3 || imgTMO->channels != 3) {
            return imgOut;
        }

        float mHDR[3], mTMO[3];

        imgHDR->getMaxVal(NULL, mHDR);
        imgTMO->getMaxVal(NULL, mTMO);

        int ind;
        float maxHDR = Arrayf::getMax(mHDR, 3, ind);
        float maxTMO = Arrayf::getMax(mTMO, 3, ind);

        if(maxHDR > 0.0f && maxTMO > 0.0f) {
            FilterColorCorrectionPouli filter;
            filter.update(maxHDR, maxTMO);
            return filter.Process(Double(imgHDR, imgTMO), imgOut);
        } else {
            return imgOut;
        }
    }

};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_COLOR_CORRECTION_POULI_HPP */

