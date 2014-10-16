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

#ifndef PIC_FILTERING_FILTER_GL_MAX_HPP
#define PIC_FILTERING_FILTER_GL_MAX_HPP

#include "gl/filtering/filter_npasses.hpp"
#include "gl/filtering/filter_non_linear_1d.hpp"

namespace pic {

/**
 * @brief The FilterGLMax class
 */
class FilterGLMax: public FilterGLNPasses
{
protected:

protected:
    FilterGLNonLinear1D *filter;
    int                 kernelSize;

public:

    /**
     * @brief FilterMax
     * @param kernelSize
     */
    FilterGLMax(int kernelSize)
    {
        filter = new FilterGLNonLinear1D(kernelSize, "max");

        InsertFilter(filter);
        InsertFilter(filter);
    }

    ~FilterGLMax()
    {
        if(filter != NULL) {
            delete filter;
            filter = NULL;
        }
    }


    /**
     * @brief Execute
     * @param imgIn
     * @param imgOut
     * @param size
     * @return
     */
    static ImageGL *Execute(ImageGL *imgIn, ImageGL *imgOut, int size)
    {
        FilterGLMax filter(size);
        return filter.Process(SingleGL(imgIn), imgOut);
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
        ImageGL imgIn(nameIn);
        imgIn.generateTextureGL(false, GL_TEXTURE_2D);
        ImageGL *imgOut = Execute(&imgIn, NULL, size);
        imgOut->loadToMemory();
        imgOut->Write(nameOut);
        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_GL_MAX_HPP */

