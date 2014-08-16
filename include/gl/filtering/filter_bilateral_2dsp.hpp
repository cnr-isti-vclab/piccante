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

#ifndef PIC_GL_FILTERING_FILTER_BILATERAL_2DSP_HPP
#define PIC_GL_FILTERING_FILTER_BILATERAL_2DSP_HPP

#include "gl/filtering/filter_bilateral_1d.hpp"

namespace pic {

class FilterGLBilateral2DSP: public FilterGLNPasses
{
protected:
    FilterGLBilateral1D		*filter;

    void InitShaders() {}
    void FragmentShader() {}

public:
    //Basic constructors
    FilterGLBilateral2DSP();
    //Init constructors
    FilterGLBilateral2DSP(float sigma_s, float sigma_r);

    //Update
    void Update(float sigma_s, float sigma_r);

    static ImageRAWGL *Execute(std::string nameIn, std::string nameOut,
                               float sigma_s, float sigma_r, int testing)
    {
        GLuint testTQ = glBeginTimeQuery();
        glEndTimeQuery(testTQ);

        ImageRAWGL imgIn(nameIn);
        imgIn.generateTextureGL(false, GL_TEXTURE_2D);

        FilterGLBilateral2DSP filter(sigma_s, sigma_r);
        ImageRAWGL *imgRet = new ImageRAWGL(1, imgIn.width, imgIn.height, 3, IMG_GPU, GL_TEXTURE_2D);

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

        std::string sign = GenBilString("S", sigma_s, sigma_r);
        std::string nameTime = FileLister::FileNumber(sign, "txt");

        FILE *file = fopen(nameTime.c_str(), "w");

        if(file != NULL) {
            fprintf(file, "%f", ms);
            fclose(file);
        }

        ImageRAWGL *imgWrite = new ImageRAWGL(1, imgIn.width, imgIn.height, 4, IMG_CPU, GL_TEXTURE_2D);
        imgWrite->readFromFBO(filter.getFbo());
        imgWrite->Write(nameOut);
        return imgRet;
    }
};

//Basic constructor
FilterGLBilateral2DSP::FilterGLBilateral2DSP(): FilterGLNPasses()
{
    target = GL_TEXTURE_2D;
}

//Constructor
FilterGLBilateral2DSP::FilterGLBilateral2DSP(float sigma_s,
        float sigma_r): FilterGLNPasses()
{
    target = GL_TEXTURE_2D;

    filter = new FilterGLBilateral1D(sigma_s, sigma_r, 0, GL_TEXTURE_2D);
    InsertFilter(filter);
    InsertFilter(filter);
}

void FilterGLBilateral2DSP::Update(float sigma_s, float sigma_r)
{
    filter->Update(sigma_s, sigma_r);
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_BILATERAL_2DSP_HPP */

