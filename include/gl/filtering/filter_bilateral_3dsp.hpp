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

#ifndef PIC_GL_FILTERING_FILTER_BILATERAL_3DSP_HPP
#define PIC_GL_FILTERING_FILTER_BILATERAL_3DSP_HPP

#include "gl/filtering/filter.hpp"

namespace pic {

class FilterGLBilateral3DSP: public FilterGLNPasses
{
protected:
    FilterGLBilateral1D		*filterS;
    FilterGLBilateral1D		*filterT;

    void InitShaders() {}
    void FragmentShader() {}

public:
    //Basic constructors
    FilterGLBilateral3DSP();
    ~FilterGLBilateral3DSP();
    //Init constructors
    FilterGLBilateral3DSP(float sigma_s, float sigma_r, float sigma_t);

    void setFrame(int frame)
    {
        filterS->setSlice(frame);
        filterT->setSlice(frame);
    }

    //Update
    void Update(float sigma_s,  float sigma_r, float sigma_t);

    static ImageGL *Execute(std::string nameIn, std::string nameOut,
                               float sigma_s, float sigma_r, float sigma_t)
    {
        return NULL;
    }
};

//Basic constructor
FilterGLBilateral3DSP::FilterGLBilateral3DSP(): FilterGLNPasses()
{
    target = GL_TEXTURE_2D_ARRAY;
    filterS = filterT = NULL;
}

FilterGLBilateral3DSP::~FilterGLBilateral3DSP()
{
    delete filterS;
    delete filterT;
}

//Constructor
FilterGLBilateral3DSP::FilterGLBilateral3DSP(float sigma_s, float sigma_r,
        float sigma_t): FilterGLNPasses()
{
    target = GL_TEXTURE_2D_ARRAY;
    filterS = new FilterGLBilateral1D(sigma_s, sigma_r, 0, target);
    filterT = new FilterGLBilateral1D(sigma_t, sigma_r, 0, target);

    InsertFilter(filterS);
    InsertFilter(filterS);
    InsertFilter(filterT);
}

void FilterGLBilateral3DSP::Update(float sigma_s, float sigma_r, float sigma_t)
{
    filterS->Update(sigma_s, sigma_r);
    filterT->Update(sigma_t, sigma_r);
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_BILATERAL_3DSP_HPP */

