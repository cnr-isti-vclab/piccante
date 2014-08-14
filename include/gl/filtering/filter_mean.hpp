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

#ifndef PIC_FILTERING_FILTER_GL_MEAN_HPP
#define PIC_FILTERING_FILTER_GL_MEAN_HPP

#include "gl/filtering/filter_npasses.hpp"

namespace pic {

/**
 * @brief The FilterGLMean class
 */
class FilterGLMean: public FilterGLNPasses
{
protected:

protected:
    FilterGLConv1D  *filter;
    ImageRAWGL      *weights;
    float           *data;
    int             size;    

public:

    /**
     * @brief FilterMean
     * @param size
     */
    FilterGLMean(int size)
    {
        data = NULL;
        weights = NULL;
        this->size = -1;

        Update(size);

        filter = new FilterGLConv1D(weights, 0, GL_TEXTURE_2D);

        InsertFilter(filter);
        InsertFilter(filter);
    }

    ~FilterGLMean()
    {
        if(filter != NULL) {
            delete filter;
            filter = NULL;
        }

        if( data != NULL) {
            delete[] data;
            data = NULL;
        }
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
            if( data != NULL) {
                delete[] data;
                data = NULL;
            }

            data = FilterConv1D::getKernelMean(size);
        }

        if(weights != NULL) {
            delete weights;
        }

        weights = new ImageRAWGL(1, size, 1, 1, data);
        weights->generateTextureGL(false);
    }

    /**
     * @brief Execute
     * @param imgIn
     * @param imgOut
     * @param size
     * @return
     */
    static ImageRAWGL *Execute(ImageRAWGL *imgIn, ImageRAWGL *imgOut, int size)
    {
        FilterGLMean filter(size);
        return filter.Process(SingleGL(imgIn), imgOut);
    }

    /**
     * @brief Execute
     * @param nameIn
     * @param nameOut
     * @param size
     * @return
     */
    static ImageRAW *Execute(std::string nameIn, std::string nameOut, int size)
    {
        ImageRAWGL imgIn(nameIn);
        imgIn.generateTextureGL(false);
        ImageRAWGL *imgOut = Execute(&imgIn, NULL, size);
        imgOut->loadToMemory();
        imgOut->Write(nameOut);
        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_GL_MEAN_HPP */

