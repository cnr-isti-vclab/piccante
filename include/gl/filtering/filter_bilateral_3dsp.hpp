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

#ifndef PIC_GL_FILTERING_FILTER_BILATERAL_3DSP_HPP
#define PIC_GL_FILTERING_FILTER_BILATERAL_3DSP_HPP

#include "gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLBilateral3DSP class
 */
class FilterGLBilateral3DSP: public FilterGLNPasses
{
protected:
    FilterGLBilateral1D		*filterS;
    FilterGLBilateral1D		*filterT;

    void InitShaders() {}
    void FragmentShader() {}

public:
    /**
     * @brief FilterGLBilateral3DSP
     */
    FilterGLBilateral3DSP();

    /**
     * @brief FilterGLBilateral3DSP
     * @param sigma_s
     * @param sigma_r
     * @param sigma_t
     */
    FilterGLBilateral3DSP(float sigma_s, float sigma_r, float sigma_t);

    ~FilterGLBilateral3DSP();

    /**
     * @brief setFrame
     * @param frame
     */
    void setFrame(int frame)
    {
        filterS->setSlice(frame);
        filterT->setSlice(frame);
    }

    /**
     * @brief Update
     * @param sigma_s
     * @param sigma_r
     * @param sigma_t
     */
    void Update(float sigma_s,  float sigma_r, float sigma_t);

    /**
     * @brief Execute
     * @param nameIn
     * @param nameOut
     * @param sigma_s
     * @param sigma_r
     * @param sigma_t
     * @return
     */
    static ImageGL *Execute(std::string nameIn, std::string nameOut,
                               float sigma_s, float sigma_r, float sigma_t)
    {
        return NULL;
    }
};

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

