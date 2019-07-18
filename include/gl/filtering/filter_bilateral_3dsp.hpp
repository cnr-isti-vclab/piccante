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

#include "../../base.hpp"
#include "../../util/std_util.hpp"
#include "../../gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLBilateral3DSP class
 */
class FilterGLBilateral3DSP: public FilterGLNPasses
{
protected:
    FilterGLBilateral1D *filterS;
    FilterGLBilateral1D *filterT;

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
     * @brief releaseAux
     */
    void releaseAux()
    {
        delete_s(filterS);
        delete_s(filterT);
    }

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
     * @brief update
     * @param sigma_s
     * @param sigma_r
     * @param sigma_t
     */
    void update(float sigma_s,  float sigma_r, float sigma_t);

    /**
     * @brief execute
     * @param nameIn
     * @param nameOut
     * @param sigma_s
     * @param sigma_r
     * @param sigma_t
     * @return
     */
    static ImageGL *execute(std::string nameIn, std::string nameOut,
                               float sigma_s, float sigma_r, float sigma_t)
    {
        return NULL;
    }
};

PIC_INLINE FilterGLBilateral3DSP::FilterGLBilateral3DSP(): FilterGLNPasses()
{
    target = GL_TEXTURE_2D_ARRAY;
    filterS = filterT = NULL;
}

PIC_INLINE FilterGLBilateral3DSP::~FilterGLBilateral3DSP()
{
    release();
}

PIC_INLINE FilterGLBilateral3DSP::FilterGLBilateral3DSP(float sigma_s, float sigma_r,
        float sigma_t): FilterGLNPasses()
{
    target = GL_TEXTURE_2D_ARRAY;
    filterS = new FilterGLBilateral1D(sigma_s, sigma_r, 0, target);
    filterT = new FilterGLBilateral1D(sigma_t, sigma_r, 0, target);

    insertFilter(filterS);
    insertFilter(filterS);
    insertFilter(filterT);
}

PIC_INLINE void FilterGLBilateral3DSP::update(float sigma_s, float sigma_r, float sigma_t)
{
    filterS->update(sigma_s, sigma_r);
    filterT->update(sigma_t, sigma_r);
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_BILATERAL_3DSP_HPP */

