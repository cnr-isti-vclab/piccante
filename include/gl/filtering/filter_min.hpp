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

#ifndef PIC_FILTERING_FILTER_GL_MIN_HPP
#define PIC_FILTERING_FILTER_GL_MIN_HPP

#include "gl/filtering/filter_npasses.hpp"
#include "gl/filtering/filter_non_linear_1d.hpp"

namespace pic {

/**
 * @brief The FilterGLMin class
 */
class FilterGLMin: public FilterGLNPasses
{
protected:

protected:
    FilterGLNonLinear1D *filter;
    int                 kernelSize;

public:

    /**
     * @brief FilterGLMin
     * @param kernelSize
     */
    FilterGLMin(int kernelSize)
    {
        filter = new FilterGLNonLinear1D(kernelSize, "min");

        InsertFilter(filter);
        InsertFilter(filter);
    }

    ~FilterGLMin()
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
        FilterGLMin filter(size);
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

#endif /* PIC_FILTERING_FILTER_GL_MIN_HPP */

