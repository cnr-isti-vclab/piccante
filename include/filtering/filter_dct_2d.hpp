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

#ifndef PIC_FILTERING_FILTER_DCT_2D_HPP
#define PIC_FILTERING_FILTER_DCT_2D_HPP

#include "../filtering/filter_npasses.hpp"
#include "../filtering/filter_dct_1d.hpp"

namespace pic {

/**
 * @brief The FilterDCT2D class
 */
class FilterDCT2D: public FilterNPasses
{
protected:
    FilterDCT1D *fltDCT1D;

public:
    /**
     * @brief FilterDCT2D
     * @param nCoeff
     * @param bForward
     */
    FilterDCT2D(int nCoeff, bool bForward)
    {
        //DCT 1D filter
        fltDCT1D = new FilterDCT1D(nCoeff, bForward);

        insertFilter(fltDCT1D);
        insertFilter(fltDCT1D);
    }

    ~FilterDCT2D()
    {
        release();

        if(fltDCT1D != NULL) {
            delete fltDCT1D;
        }

        fltDCT1D = NULL;
    }

    /**
     * @brief SetForward
     */
    void setForward()
    {
        fltDCT1D->setForward();
    }

    /**
     * @brief setInverse
     */
    void setInverse()
    {
        fltDCT1D->setInverse();
    }

    /**
     * @brief transform
     * @param imgIn
     * @param imgOut
     * @param nCoeff
     * @return
     */
    static Image *transform(Image *imgIn, Image *imgOut, int nCoeff)
    {
        FilterDCT2D filter(nCoeff, true);
        return filter.Process(Single(imgIn), imgOut);
    }

    /**
     * @brief inverse
     * @param imgIn
     * @param imgOut
     * @param nCoeff
     * @return
     */
    static Image *inverse(Image *imgIn, Image *imgOut, int nCoeff)
    {
        FilterDCT2D filter(nCoeff, false);
        return filter.Process(Single(imgIn), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_DCT_2D_HPP */

