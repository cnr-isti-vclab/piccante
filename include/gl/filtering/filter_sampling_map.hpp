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

#ifndef PIC_GL_FILTERING_FILTER_SAMPLING_MAP_HPP
#define PIC_GL_FILTERING_FILTER_SAMPLING_MAP_HPP

#include "gl/filtering/filter_npasses.hpp"
#include "gl/filtering/filter_gradient.hpp"
#include "gl/filtering/filter_sigmoid_tmo.hpp"
#include "gl/filtering/filter_sampler_2d.hpp"
#include "gl/filtering/filter_gaussian_2d.hpp"

namespace pic {

class FilterGLSamplingMap: public FilterGLNPasses
{
protected:
    float					scale, sigma;

    FilterGLGradient		*filterG;
    FilterGLSigmoidTMO		*filterS;
    FilterGLSampler2D		*filterD;
    FilterGLGaussian2D		*filterG2D;

    void InitShaders() {}
    void FragmentShader() {}
    void Setup(float sigma, float scale);
    ImageRAWGL *SetupAuxN(ImageRAWGLVec imgIn, ImageRAWGL *imgOut);

public:
    //Basic constructors
    FilterGLSamplingMap(float sigma);
    FilterGLSamplingMap(float sigma, float scale);
    ~FilterGLSamplingMap();

    float getScale()
    {
        return scale;
    }

    Fbo *getFbo();

    static ImageRAWGL *Execute(std::string nameIn, std::string nameOut, float sigma)
    {
        ImageRAWGL imgIn(nameIn);
        imgIn.generateTextureGL(false);

        FilterGLSamplingMap filter(sigma);

        GLuint testTQ1 = glBeginTimeQuery();
        ImageRAWGL *imgRet = filter.Process(SingleGL(&imgIn), NULL);

        GLuint64EXT timeVal = glEndTimeQuery(testTQ1);
        printf("Sampling Map Filter on GPU time: %f ms\n", (timeVal) / 100000000.0f);

        ImageRAWGL *imgOut = new ImageRAWGL(1, imgRet->width, imgRet->height, 1,
                                            IMG_CPU);
        imgOut->readFromFBO(filter.getFbo(), GL_RED);
        printf("%f %f\n", imgOut->getMaxVal(), imgOut->getMinVal());
        imgOut->Write(nameOut);

        return imgRet;
    }
};

//Basic constructor
FilterGLSamplingMap::FilterGLSamplingMap(float sigma): FilterGLNPasses()
{
    target = GL_TEXTURE_2D;
    float rateScale = 2.0f;
    Setup(rateScale, rateScale / sigma);
}

FilterGLSamplingMap::FilterGLSamplingMap(float sigma,
        float scale): FilterGLNPasses()
{
    target = GL_TEXTURE_2D;
    Setup(sigma * scale, scale);
}

FilterGLSamplingMap::~FilterGLSamplingMap()
{
    delete filterG;
    delete filterS;
    delete filterD;
    delete filterG2D;
}

//Basic constructor
void FilterGLSamplingMap::Setup(float sigma, float scale)
{
    this->sigma = sigma;
    this->scale = scale;
    filterS = new FilterGLSigmoidTMO();
    filterG = new FilterGLGradient();
    filterG2D = new FilterGLGaussian2D(sigma);

    //	filterG2D = new FilterGLGaussian2D(sigma*scale);
    filterD = new FilterGLSampler2D(scale);

    InsertFilter(filterD);
    InsertFilter(filterS);
    InsertFilter(filterG);
    InsertFilter(filterG2D);
}

Fbo *FilterGLSamplingMap::getFbo()
{
    if(filters.size() <= 0) {
        return NULL;
    }

    return filters[filters.size() - 1]->getFbo();
}

ImageRAWGL *FilterGLSamplingMap::SetupAuxN(ImageRAWGLVec imgIn,
        ImageRAWGL *imgOut)
{
    if(imgOut == NULL) {
        imgOut = new ImageRAWGL(imgIn[0]->frames, imgIn[0]->width * scale,
                                imgIn[0]->height * scale, imgIn[0]->channels, IMG_GPU);
    }

    /*if(fbo==NULL)
    	fbo = new Fbo();
    fbo->create(imgIn[0]->width*scale,imgIn[0]->height*scale,imgIn[0]->frames, false, imgOut->getTexture());
    for(int i=0;i<filters.size();i++)
    	filters[i]->setFbo(fbo);*/

    return imgOut;
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_SAMPLING_MAP_HPP */

