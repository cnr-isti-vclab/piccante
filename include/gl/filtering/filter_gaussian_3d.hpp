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

#ifndef PIC_GL_FILTERING_FILTER_GAUSSIAN_3D_HPP
#define PIC_GL_FILTERING_FILTER_GAUSSIAN_3D_HPP

#include "gl/filtering/filter_npasses.hpp"
#include "gl/filtering/filter_gaussian_1d.hpp"

namespace pic {

class FilterGLGaussian3D: public FilterGLNPasses
{
protected:
    FilterGLGaussian1D		*filter;

    void InitShaders() {}
    void FragmentShader() {}

public:
    /**
     * @brief FilterGLGaussian3D
     */
    FilterGLGaussian3D();

    /**
     * @brief FilterGLGaussian3D
     * @param sigma
     */
    FilterGLGaussian3D(float sigma);

    /**
     * @brief Update
     * @param sigma
     */
    void Update(float sigma);
};

FilterGLGaussian3D::FilterGLGaussian3D(): FilterGLNPasses()
{
    target = GL_TEXTURE_3D;
}

FilterGLGaussian3D::FilterGLGaussian3D(float sigma): FilterGLNPasses()
{
    filter = new FilterGLGaussian1D(sigma, 0, GL_TEXTURE_3D);
    target = GL_TEXTURE_3D;

    InsertFilter(filter);
    InsertFilter(filter);
    InsertFilter(filter);
}

void FilterGLGaussian3D::Update(float sigma)
{
    filter->Update(sigma);
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_GAUSSIAN_3D_HPP */

