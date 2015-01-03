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

#ifndef PIC_FILTERING_FILTER_MEAN_HPP
#define PIC_FILTERING_FILTER_MEAN_HPP

#include "filtering/filter_npasses.hpp"

namespace pic {

/**
 * @brief The FilterMean class
 */
class FilterMean: public FilterNPasses
{
protected:

protected:
    FilterConv1D    *filter;
    float           *data;
    int             size;    

public:

    /**
     * @brief FilterMean
     * @param size
     */
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

    /**
     * @brief Update
     * @param size
     */
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

    /**
     * @brief Execute
     * @param imgIn
     * @param imgOut
     * @param size
     * @return
     */
    static Image *Execute(Image *imgIn, Image *imgOut, int size)
    {
        FilterMean filter(size);
        return filter.ProcessP(Single(imgIn), imgOut);
    }

    /**
     * @brief Execute
     * @param nameIn
     * @param nameOut
     * @param size
     * @return
     */
    static Image *Execute(std::string nameIn, std::string nameOut, int size)
    {
        Image imgIn(nameIn);
        Image *imgOut = Execute(&imgIn, NULL, size);
        imgOut->Write(nameOut);
        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_MEAN_HPP */

