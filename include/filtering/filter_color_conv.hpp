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

#include "../filtering/filter.hpp"
#include "../colors/color_conv.hpp"
#include "../colors/color_conv_rgb_to_xyz.hpp"
#include "../colors/color_conv_xyz_to_logluv.hpp"
#include "../colors/color_conv_xyz_to_cielab.hpp"

namespace pic {

struct ColorConvTransform
{
    ColorConv *f;
    bool bDirection;
};

/**
 * @brief The FilterColorConv class
 */
class FilterColorConv: public Filter
{
protected:
    std::vector<ColorConvTransform> list;
    bool bDirection;
    unsigned int n;
    bool bEven;

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

        int channels = src[0]->channels;

        float *tmpCol = new float [channels];
        float *tmp[2];

        tmp[1] = tmpCol;

        for(int j = box->y0; j < box->y1; j++) {

            for(int i = box->x0; i < box->x1; i++) {

                float *dataIn  = (*src[0])(i, j);
                float *dataOut = (*dst) (i, j);

                if(bEven) {
                    tmp[1] = dataOut;
                    tmp[0] = tmpCol;
                } else {
                    tmp[0] = dataOut;
                    tmp[1] = tmpCol;
                }

                if(bDirection) { //direct color transform
                    list[0].f->transform(dataIn, tmp[0], list[0].bDirection);
                    for(unsigned int k = 1; k < n; k++) {
                        list[k].f->transform(tmp[(k + 1) % 2], tmp[k % 2], list[k].bDirection);
                    }
                } else { //inverse color transform
                    list[n - 1].f->transform(dataIn, tmp[0], !list[n - 1].bDirection);
                    for(unsigned int k = 1; k < n; k++) {
                        list[n - k - 1].f->transform(tmp[(k + 1) % 2], tmp[k % 2], !list[n - k - 1].bDirection);
                    }
                }
            }
        }

        delete[] tmpCol;
    }

public:

    /**
     * @brief FilterColorConv
     */
    FilterColorConv() : Filter()
    {
        this->bDirection = true;
        n = -1;
    }

    /**
     * @brief insertColorConv
     * @param transform
     * @param bDirection
     */
    void insertColorConv(ColorConv *transform, bool bDirection)
    {
        if(transform != NULL) {
            ColorConvTransform entry;
            entry.f = transform;
            entry.bDirection = bDirection;

            list.push_back(entry);
        }

        n = int(list.size());
        bEven = (n % 2) == 0;
    }

    /**
     * @brief update
     * @param bDirection
     */
    void update(bool bDirection)
    {
        this->bDirection = bDirection;
    }

    /**
     * @brief fromRGBtoXYZ
     * @param imgIn
     * @param imgOut
     * @return
     */
    static Image *fromRGBtoXYZ(Image *imgIn, Image *imgOut)
    {
        ColorConvRGBtoXYZ    cc_from_RGB_to_XYZ;

        FilterColorConv flt;

        flt.insertColorConv(&cc_from_RGB_to_XYZ,    true);
        return flt.Process(Single(imgIn), imgOut);
    }

    /**
     * @brief fromRGBtoCIELAB
     * @param imgIn
     * @param imgOut
     * @return
     */
    static Image *fromRGBtoCIELAB(Image *imgIn, Image *imgOut)
    {
        ColorConvRGBtoXYZ    cc_from_RGB_to_XYZ;
        ColorConvXYZtoCIELAB cc_from_XYZ_to_CIELAB;

        FilterColorConv flt;

        flt.insertColorConv(&cc_from_RGB_to_XYZ,    true);
        flt.insertColorConv(&cc_from_XYZ_to_CIELAB, true);

        return flt.Process(Single(imgIn), imgOut);
    }

    /**
    * @brief fromRGBtoLogLuv
    * @param imgIn
    * @param imgOut
    * @return
    */
    static Image *fromRGBtoLogLuv(Image *imgIn, Image *imgOut)
    {
        ColorConvRGBtoXYZ    cc_from_RGB_to_XYZ;
        ColorConvXYZtoLogLuv cc_from_XYZ_to_LogLuv;

        FilterColorConv flt;

        flt.insertColorConv(&cc_from_RGB_to_XYZ, true);
        flt.insertColorConv(&cc_from_XYZ_to_LogLuv, true);

        return flt.Process(Single(imgIn), imgOut);
    }

    /**
     * @brief fromCIELABtoRGB
     * @param imgIn
     * @param imgOut
     * @return
     */
    static Image *fromCIELABtoRGB(Image *imgIn, Image *imgOut)
    {
        ColorConvRGBtoXYZ    cc_from_RGB_to_XYZ;
        ColorConvXYZtoCIELAB cc_from_XYZ_to_CIELAB;

        FilterColorConv flt;

        flt.insertColorConv(&cc_from_XYZ_to_CIELAB, false);
        flt.insertColorConv(&cc_from_RGB_to_XYZ,    false);

        return flt.Process(Single(imgIn), imgOut);
    }

    /**
     * @brief fromCIELABtoRGB2
     * @param imgIn
     * @param imgOut
     * @return
     */
    static Image *fromCIELABtoRGB2(Image *imgIn, Image *imgOut)
    {
        ColorConvRGBtoXYZ    cc_from_RGB_to_XYZ;
        ColorConvXYZtoCIELAB cc_from_XYZ_to_CIELAB;

        FilterColorConv flt;

        flt.insertColorConv(&cc_from_RGB_to_XYZ,    true);
        flt.insertColorConv(&cc_from_XYZ_to_CIELAB, true);

        flt.update(false);

        return flt.Process(Single(imgIn), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_COLOR_CONV_HPP */

