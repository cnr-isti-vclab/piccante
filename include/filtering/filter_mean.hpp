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

#ifndef PIC_FILTERING_FILTER_MEAN_HPP
#define PIC_FILTERING_FILTER_MEAN_HPP

#include "filtering/filter_npasses.hpp"

namespace pic {

class FilterMean: public FilterNPasses
{
protected:

protected:
    FilterConv1D    *filter;
    float           *data;
    int             size;    

public:
    //Basic constructor
    FilterMean(int size)
    {
        data = NULL;
        this->size = -1;

        Update(size);

        filter = new FilterConv1D(data, size);

        InsertFilter(filter);
        InsertFilter(filter);
    }

    ~FilterMean()
    {
        Destroy();

        if(filter != NULL) {
            delete filter;
        }

        if( data != NULL)
            delete[] data;
    }

    void Update(int size)
    {
        if(size < 1)
        {
            size = 3;
        }

        if(this->size != size)
        {
            this->size = size;
            if( data != NULL)
                delete[] data;

            data = FilterConv1D::getKernelMean(size);
        }        
    }

    static ImageRAW *Execute(ImageRAW *imgIn, ImageRAW *imgOut, int size)
    {
        FilterMean filter(size);
        return filter.ProcessP(Single(imgIn), imgOut);
    }

    static ImageRAW *Execute(std::string nameIn, std::string nameOut, int size)
    {
        ImageRAW imgIn(nameIn);
        ImageRAW *imgOut = Execute(&imgIn, NULL, size);
        imgOut->Write(nameOut);
        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_MEAN_HPP */

