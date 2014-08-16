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
#include "gl/filtering/filter_conv_1d.hpp"

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
    int             kernelSize;

public:

    /**
     * @brief FilterMean
     * @param kernelSize
     */
    FilterGLMean(int kernelSize)
    {
        data = NULL;
        weights = NULL;
        this->kernelSize = -1;

        Update(kernelSize);

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
    void Update(int kernelSize)
    {
        if(kernelSize < 1)
        {
            kernelSize = 3;
        }

        if(this->kernelSize != kernelSize)
        {
            this->kernelSize = kernelSize;
            if( data != NULL) {
                delete[] data;
                data = NULL;
            }

            data = FilterConv1D::getKernelMean(kernelSize);
        }

        if(weights != NULL) {
            delete weights;
        }

        weights = new ImageRAWGL(1, kernelSize, 1, 1, data);
        weights->generateTextureGL(false, GL_TEXTURE_2D);
    }

    /**
     * @brief Execute
     * @param imgIn
     * @param imgOut
     * @param kernelSize
     * @return
     */
    static ImageRAWGL *Execute(ImageRAWGL *imgIn, ImageRAWGL *imgOut, int kernelSize)
    {
        FilterGLMean filter(kernelSize);
        return filter.Process(SingleGL(imgIn), imgOut);
    }

    /**
     * @brief Execute
     * @param nameIn
     * @param nameOut
     * @param kernelSize
     * @return
     */
    static ImageRAW *Execute(std::string nameIn, std::string nameOut, int kernelSize)
    {
        ImageRAWGL imgIn(nameIn);
        imgIn.generateTextureGL(false, GL_TEXTURE_2D);
        ImageRAWGL *imgOut = Execute(&imgIn, NULL, kernelSize);
        imgOut->loadToMemory();
        imgOut->Write(nameOut);
        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_GL_MEAN_HPP */

