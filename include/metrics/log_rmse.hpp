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

#ifndef PIC_METRICS_LOG_RMSE_HPP
#define PIC_METRICS_LOG_RMSE_HPP

#include <math.h>
#include "image_raw.hpp"
#include "metrics/base.hpp"

namespace pic {

//RMSE in the log domain
double logRMSE(ImageRAW *ori, ImageRAW *cmp)
{
    if(ori == NULL || cmp == NULL) {
        return -2.0;
    }

    if(!ori->SimilarType(cmp)) {
        return -1.0;
    }

    int size = ori->size();
    int counter = 0;

    double acc = 0.0;
    double val;

    for(int i = 0; i < size; i += ori->channels) {
        for(int j = 0; j < ori->channels; j++) {
            if(ori->data[i + j] > 0.0f && cmp->data[i + j] > 0.0f) {
                val  = log(ori->data[i + j] / cmp->data[i + j]);
                acc += val * val;
                counter++;
            }
        }
    }

    if(counter > 0) {
        acc = acc / double(counter);
        return sqrt(acc);
    } else {
        return -3.0;
    }
}

} // end namespace pic

#endif /* PIC_METRICS_LOG_RMSE_HPP */

