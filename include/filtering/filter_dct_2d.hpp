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

#ifndef PIC_FILTERING_FILTER_DCT_2D_HPP
#define PIC_FILTERING_FILTER_DCT_2D_HPP

#include "filtering/filter_npasses.hpp"
#include "filtering/filter_dct_1d.hpp"

namespace pic {

class FilterDCT2D: public FilterNPasses
{
protected:
    FilterDCT1D *fltDCT1D;

public:
    //Basic constructor
    FilterDCT2D(int nCoeff, bool bForward)
    {
        //DCT 1D filter
        fltDCT1D = new FilterDCT1D(nCoeff, bForward);

        InsertFilter(fltDCT1D);
        InsertFilter(fltDCT1D);
    }

    ~FilterDCT2D()
    {
        Destroy();

        if(fltDCT1D != NULL) {
            delete fltDCT1D;
        }

        fltDCT1D = NULL;
    }

    void SetForward()
    {
        fltDCT1D->SetForward();
    }
    void SetInverse()
    {
        fltDCT1D->SetInverse();
    }

    static ImageRAW *Transform(ImageRAW *imgIn, ImageRAW *imgOut, int nCoeff)
    {
        FilterDCT2D filter(nCoeff, true);
        return filter.Process(Single(imgIn), imgOut);
    }

    static ImageRAW *Inverse(ImageRAW *imgIn, ImageRAW *imgOut, int nCoeff)
    {
        FilterDCT2D filter(nCoeff, false);
        return filter.ProcessP(Single(imgIn), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_DCT_2D_HPP */

