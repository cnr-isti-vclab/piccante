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

#ifndef PIC_GL_FILTERING_FILTER_SAMPLING_MAP_HPP
#define PIC_GL_FILTERING_FILTER_SAMPLING_MAP_HPP

#include "gl/filtering/filter_npasses.hpp"
#include "gl/filtering/filter_gradient.hpp"
#include "gl/filtering/filter_sigmoid_tmo.hpp"
#include "gl/filtering/filter_sampler_2d.hpp"
#include "gl/filtering/filter_gaussian_2d.hpp"

namespace pic {

/**
 * @brief The FilterGLSamplingMap class
 */
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

    /**
     * @brief Setup
     * @param sigma
     * @param scale
     */
    void Setup(float sigma, float scale);

    /**
     * @brief SetupAuxN
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageGL *SetupAuxN(ImageGLVec imgIn, ImageGL *imgOut);

public:
    /**
     * @brief FilterGLSamplingMap
     * @param sigma
     */
    FilterGLSamplingMap(float sigma);

    /**
     * @brief FilterGLSamplingMap
     * @param sigma
     * @param scale
     */
    FilterGLSamplingMap(float sigma, float scale);

    ~FilterGLSamplingMap();

    /**
     * @brief getScale
     * @return
     */
    float getScale()
    {
        return scale;
    }

    /**
     * @brief getFbo
     * @return
     */
    Fbo *getFbo();

    /**
     * @brief Execute
     * @param nameIn
     * @param nameOut
     * @param sigma
     * @return
     */
    static ImageGL *Execute(std::string nameIn, std::string nameOut, float sigma)
    {
        ImageGL imgIn(nameIn);
        imgIn.generateTextureGL(false, GL_TEXTURE_2D);

        FilterGLSamplingMap filter(sigma);

        GLuint testTQ1 = glBeginTimeQuery();
        ImageGL *imgRet = filter.Process(SingleGL(&imgIn), NULL);

        GLuint64EXT timeVal = glEndTimeQuery(testTQ1);
        printf("Sampling Map Filter on GPU time: %f ms\n", (timeVal) / 100000000.0f);

        imgRet->loadToMemory();
        imgRet->Write(nameOut);

        return imgRet;
    }
};

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

FilterGLSamplingMap::~FilterGLSamplingMap()
{
    delete filterG;
    delete filterS;
    delete filterD;
    delete filterG2D;
}

Fbo *FilterGLSamplingMap::getFbo()
{
    if(filters.size() <= 0) {
        return NULL;
    }

    return filters[filters.size() - 1]->getFbo();
}

ImageGL *FilterGLSamplingMap::SetupAuxN(ImageGLVec imgIn,
        ImageGL *imgOut)
{
    if(imgOut == NULL) {
        imgOut = new ImageGL(    imgIn[0]->frames,
                                    int(imgIn[0]->widthf  * scale),
                                    int(imgIn[0]->heightf * scale),
                                    imgIn[0]->channels, IMG_GPU, GL_TEXTURE_2D);
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

