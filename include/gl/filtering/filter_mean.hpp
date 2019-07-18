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

#ifndef PIC_FILTERING_FILTER_GL_MEAN_HPP
#define PIC_FILTERING_FILTER_GL_MEAN_HPP

#include "../../util/std_util.hpp"

#include "../../gl/filtering/filter_npasses.hpp"
#include "../../gl/filtering/filter_conv_1d.hpp"

namespace pic {

/**
 * @brief The FilterGLMean class
 */
class FilterGLMean: public FilterGLNPasses
{
protected:

protected:
    FilterGLConv1D *filter;
    ImageGL *weights;
    float *data;
    int kernelSize;

public:

    /**
     * @brief FilterMean
     * @param kernelSize
     */
    FilterGLMean(int kernelSize) : FilterGLNPasses()
    {
        data = NULL;
        weights = NULL;
        this->kernelSize = -1;

        update(kernelSize);

        filter = new FilterGLConv1D(weights, 0, GL_TEXTURE_2D);

        insertFilter(filter);
        insertFilter(filter);
    }

    ~FilterGLMean()
    {
        release();
        delete_s(filter);
        delete_vec_s(data);
    }

    /**
     * @brief update
     * @param size
     */
    void update(int kernelSize)
    {
        kernelSize = kernelSize > 0 ? kernelSize : 3;

        if(this->kernelSize != kernelSize)
        {
            this->kernelSize = kernelSize;

            data = delete_vec_s(data);
            data = FilterConv1D::getKernelMean(kernelSize);
        }

        weights = delete_s(weights);

        weights = new ImageGL(1, kernelSize, 1, 1, data);
        weights->generateTextureGL(GL_TEXTURE_2D, GL_FLOAT, false);
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param kernelSize
     * @return
     */
    static ImageGL *execute(ImageGL *imgIn, ImageGL *imgOut, int kernelSize)
    {
        FilterGLMean filter(kernelSize);
        return filter.Process(SingleGL(imgIn), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_GL_MEAN_HPP */

