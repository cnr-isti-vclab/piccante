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

#ifndef PIC_FILTERING_FILTER_RADIAL_BASIS_FUNCTION
#define PIC_FILTERING_FILTER_RADIAL_BASIS_FUNCTION

namespace pic {

#include "../algorithms/radial_basis_function.hpp"

/**
 * @brief The FilterRadialBasisFunction class
 */
class FilterRadialBasisFunction: public Filter
{
protected:

    RadialBasisFunction *rbf;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box)
    {
        if(rbf == NULL || src.size() < 1) {
            return;
        }

        int channels = src[0]->channels;

        if(rbf->nDim != channels) {
            return;
        }

        for(int j = box->y0; j < box->y1; j++) {

            for(int i = box->x0; i < box->x1; i++) {

                float *dataIn  = (*src[0]) (i, j);
                float *dataOut = (*dst)    (i, j);

                float out = rbf->eval(dataIn);
                dataOut[0] = out;
            }
        }
    }

    /**
     * @brief SetupAux
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *SetupAux(ImageVec imgIn, Image *imgOut)
    {
        if(imgOut == NULL) {
            imgOut = new Image(1, imgIn[0]->width, imgIn[0]->height, 1);
        } else {
            if((imgIn[0]->width  != imgOut->width)  ||
               (imgIn[0]->height != imgOut->height) ||
               (imgOut->channels != 1)) {
                imgOut = new Image(1, imgIn[0]->width, imgIn[0]->height, 1);
            }
        }

        return imgOut;
    }

public:

    /**
     * @brief FilterRadialBasisFunction
     */
    FilterRadialBasisFunction() : Filter()
    {
        rbf = NULL;
    }

    /**
      * @brief update
      * @param rbf
      */
    void update(RadialBasisFunction *rbf)
    {
        if(rbf != NULL) {
            this->rbf = rbf;
        }
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_RADIAL_BASIS_FUNCTION */

