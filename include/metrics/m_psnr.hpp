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

#ifndef PIC_METRICS_M_PSNR_HPP
#define PIC_METRICS_M_PSNR_HPP

#include <math.h>
#include "image_raw.hpp"
#include "metrics/base.hpp"
#include "util/indexed_array.hpp"
#include "metrics/mse.hpp"

namespace pic {

/**mPSNR: multiple-exposure peak signal-to-noise ratio*/
double mPSNR(ImageRAW *ori, ImageRAW *cmp, int minFstop, int maxFstop)
{
    if(ori == NULL || cmp == NULL) {
        return -2.0;
    }

    if(!ori->SimilarType(cmp)) {
        return -1.0;
    }

    //TO DO: Calculate fstop
    if(minFstop == maxFstop) {
        ImageRAW *oriLum = FilterLuminance::Execute(ori, NULL, LT_CIE_LUMINANCE);
        ImageRAW *cmpLum = FilterLuminance::Execute(cmp, NULL, LT_CIE_LUMINANCE);

        int nData = oriLum->width * oriLum->height;

        IntCoord coordO = IndexedArray::findSimple(oriLum->data, nData, IndexedArray::bFuncNotNeg);
        IntCoord coordC = IndexedArray::findSimple(cmpLum->data, nData, IndexedArray::bFuncNotNeg);

        float commonMax = MIN(IndexedArray::max(oriLum->data, coordO),
                              IndexedArray::max(cmpLum->data, coordC));

        float commonMin = MAX(IndexedArray::min(oriLum->data, coordO),
                              IndexedArray::min(cmpLum->data, coordC));

        float tminFstop = logf(commonMin) / logf(2.0f);
        float tmaxFstop = logf(commonMax) / logf(2.0f);

        minFstop = int(lround(tminFstop));
        maxFstop = int(lround(tmaxFstop));

        int halfFstops = (maxFstop - minFstop + 1) / 2;
        minFstop = -halfFstops + 1;
        maxFstop =  halfFstops - 1;

        if(minFstop == maxFstop) {
            minFstop--;
            maxFstop++;
        }
    }

    #ifdef PIC_DEBUG
        printf("-------------------------------------------------------\n");
        printf("-- mPSNR:\n");
        printf("-- min F-stop: %d \t max F-stop: %d\n", minFstop, maxFstop);
    #endif

    double aMSE = 0.0;

    for(int i = minFstop; i <= maxFstop; i++) {
        double tmp = MSE(ori, cmp, 2.2f, float(i));
        #ifdef PIC_DEBUG
            printf("-- Pass: %d \t MSE: %g\n", i, tmp);
       #endif
       aMSE += tmp;
    }

    aMSE /= double(maxFstop - minFstop + 1);

    double MSEconst = double(ori->channels) * 65025.0; //255*255;
    float ret = float(10.0 * log10(MSEconst / aMSE));

    #ifdef PIC_DEBUG
        printf("-- value: %f\n", ret);
        printf("-------------------------------------------------------");
    #endif

    return ret;
}

} // end namespace pic

#endif /* PIC_METRICS_M_PSNR_HPP */

