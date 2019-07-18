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

#ifndef PIC_GL_FILTERING_FILTER_BILATERAL_2DSP_HPP
#define PIC_GL_FILTERING_FILTER_BILATERAL_2DSP_HPP

#include "../../util/std_util.hpp"

#include "../../gl/filtering/filter_bilateral_1d.hpp"

namespace pic {

/**
 * @brief The FilterGLBilateral2DSP class provides
 * an approximated 2D bilateral filter using two
 * 1D bilateral filtes; i.e. using the separable
 * approximation.
 */
class FilterGLBilateral2DSP: public FilterGLNPasses
{
protected:
    FilterGLBilateral1D *filter;

public:
    /**
     * @brief FilterGLBilateral2DSP
     */
    FilterGLBilateral2DSP();

    /**
     * @brief FilterGLBilateral2DSP
     * @param sigma_s
     * @param sigma_r
     */
    FilterGLBilateral2DSP(float sigma_s, float sigma_r);

    ~FilterGLBilateral2DSP();

    /**
     * @brief releaseAux
     */
    void releaseAux()
    {
       delete_s(filter);
    }

    /**
    * @brief update
    * @param sigma_s
    * @param sigma_r
    */
    void update(float sigma_s, float sigma_r);

    /**
     * @brief execute
     * @param nameIn
     * @param nameOut
     * @param sigma_s
     * @param sigma_r
     * @param testing
     * @return
     */
    static ImageGL *execute(std::string nameIn, std::string nameOut,
                               float sigma_s, float sigma_r, int testing)
    {
        GLuint testTQ = glBeginTimeQuery();
        glEndTimeQuery(testTQ);

        ImageGL imgIn(nameIn);
        imgIn.generateTextureGL(GL_TEXTURE_2D, GL_FLOAT, false);

        FilterGLBilateral2DSP filter(sigma_s, sigma_r);
        ImageGL *imgRet = new ImageGL(1, imgIn.width, imgIn.height, 3, IMG_GPU, GL_TEXTURE_2D);

        GLuint testTQ1;

        if(testing > 1) {
            filter.Process(SingleGL(&imgIn), imgRet);

            testTQ1 = glBeginTimeQuery();

            for(int i = 0; i < testing; i++) {
                filter.Process(SingleGL(&imgIn), imgRet);
            }
        } else {
            testTQ1 = glBeginTimeQuery();
            filter.Process(SingleGL(&imgIn), imgRet);
        }

        GLuint64EXT timeVal = glEndTimeQuery(testTQ1);
        double ms = double(timeVal) / (double(testing) * 1000000.0);
        printf("Separate Bilateral Filter on GPU time: %f ms\n", ms);

        std::string sign = genBilString("S", sigma_s, sigma_r);
        std::string nameTime = FileLister::getFileNumber(sign, "txt");

        FILE *file = fopen(nameTime.c_str(), "w");

        if(file != NULL) {
            fprintf(file, "%f", ms);
            fclose(file);
        }

        ImageGL *imgWrite = new ImageGL(1, imgIn.width, imgIn.height, 4, IMG_CPU, GL_TEXTURE_2D);
        imgWrite->readFromFBO(filter.getFbo());
        imgWrite->Write(nameOut);
        return imgRet;
    }
};

PIC_INLINE FilterGLBilateral2DSP::FilterGLBilateral2DSP(): FilterGLNPasses()
{
    target = GL_TEXTURE_2D;

    filter = new FilterGLBilateral1D(1.0f, 0.01f, 0, GL_TEXTURE_2D);
    insertFilter(filter);
    insertFilter(filter);
}

PIC_INLINE FilterGLBilateral2DSP::FilterGLBilateral2DSP(float sigma_s,
        float sigma_r): FilterGLNPasses()
{
    target = GL_TEXTURE_2D;

    filter = new FilterGLBilateral1D(sigma_s, sigma_r, 0, GL_TEXTURE_2D);
    insertFilter(filter);
    insertFilter(filter);
}

PIC_INLINE FilterGLBilateral2DSP::~FilterGLBilateral2DSP()
{
    release();
}

PIC_INLINE void FilterGLBilateral2DSP::update(float sigma_s, float sigma_r)
{
    filter->update(sigma_s, sigma_r);
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_BILATERAL_2DSP_HPP */

