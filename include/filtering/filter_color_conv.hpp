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

#ifndef PIC_FILTERING_FILTER_COLOR_CONV_HPP
#define PIC_FILTERING_FILTER_COLOR_CONV_HPP

#include "filtering/filter.hpp"
#include "colors/color_conv.hpp"
#include "colors/color_conv_rgb_to_xyz.hpp"
#include "colors/color_conv_xyz_to_logluv.hpp"
#include "colors/color_conv_xyz_to_cielab.hpp"

namespace pic {

/**
 * @brief The FilterColorConv class
 */
class FilterColorConv: public Filter
{
protected:
    std::vector<ColorConv *>    conv_list;
    unsigned int				n;
    bool						bDirect;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box)
    {
        if(n < 1) {
            return;
        }

        int channels	= src[0]->channels;

        float *tmpCol = new float [channels];
        float *tmp[2];

        tmp[1] = tmpCol;

        bool bEven = (n % 2) == 0;

        for(int j = box->y0; j < box->y1; j++) {

            for(int i = box->x0; i < box->x1; i++) {

                float *dataIn  = (*src[0]) (i, j);
                float *dataOut = (*dst)    (i, j);

                if(bEven) {
                    tmp[1] = dataOut;
                    tmp[0] = tmpCol;
                } else {
                    tmp[0] = dataOut;
                    tmp[1] = tmpCol;
                }

                if(bDirect) { //Direct color transform                    
                    conv_list[0]->direct(dataIn, tmp[0]);
                    for(unsigned int k = 1; k < n; k++) {
                        conv_list[k]->direct(tmp[(k + 1) % 2], tmp[k % 2]);
                    }

                } else { //Inverse color transform
                    conv_list[n - 1]->inverse(dataIn, tmp[0]);
                    for(unsigned int k = 1; k < n; k++) {
                        conv_list[n - k - 1]->inverse(tmp[(k + 1) % 2], tmp[k % 2]);
                    }
                }
            }
        }

        delete[] tmpCol;
    }

public:

    /**
     * @brief FilterColorConv
     * @param conv
     * @param bDirect
     */
    FilterColorConv(ColorConv *conv, bool bDirect)
    {
        this->bDirect = bDirect;
        insertColorConv(conv);
    }

    /**
     * @brief insertColorConv
     * @param conv
     */
    void insertColorConv(ColorConv *conv)
    {
        if(conv != NULL) {
            conv_list.push_back(conv);
        }

        n = conv_list.size();
    }

    /**
     * @brief Update
     * @param bDirect
     */
    void Update(bool bDirect)
    {
        this->bDirect = bDirect;
    }

    /**
     * @brief RGBtoLogLuv
     * @param imgIn
     * @param imgOut
     * @param bDirect
     * @return
     */
    static Image *RGBtoLogLuv(Image *imgIn, Image *imgOut,
                                        bool bDirect)
    {
        ColorConvRGBtoXYZ       csXYZ;
        ColorConvXYZtoLogLuv    csLogLuv;

        FilterColorConv flt(NULL, bDirect);

        flt.insertColorConv(&csXYZ);
        flt.insertColorConv(&csLogLuv);

        return flt.Process(Single(imgIn), imgOut);
    }

    /**
     * @brief RGBtoCIELAB
     * @param imgIn
     * @param imgOut
     * @param bDirect
     * @return
     */
    static Image *RGBtoCIELAB(Image *imgIn, Image *imgOut,
                                        bool bDirect)
    {
        ColorConvRGBtoXYZ       csXYZ;
        ColorConvXYZtoCIELAB    csCIELAB;

        FilterColorConv flt(NULL, bDirect);

        flt.insertColorConv(&csXYZ);
        flt.insertColorConv(&csCIELAB);

        return flt.Process(Single(imgIn), imgOut);
    }

    /**
     * @brief ExectueCIELABtoRGB
     * @param imgIn
     * @param imgOut
     * @param bDirect
     * @return
     */
    static Image *CIELABtoRGB(Image *imgIn, Image *imgOut,
                                        bool bDirect)
    {
        ColorConvRGBtoXYZ       csXYZ;
        ColorConvXYZtoCIELAB    csCIELAB;

        FilterColorConv flt(NULL, bDirect);

        flt.insertColorConv(&csCIELAB);
        flt.insertColorConv(&csXYZ);

        return flt.Process(Single(imgIn), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_COLOR_CONV_HPP */

