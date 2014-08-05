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

#ifndef PIC_GL_FILTERING_FILTER_NPASSES_HPP
#define PIC_GL_FILTERING_FILTER_NPASSES_HPP

#include "gl/filtering/filter.hpp"

namespace pic {

class FilterGLNPasses: public FilterGL
{
protected:
    ImageRAWGL *imgTmp[2];

    void InitShaders() {}
    void FragmentShader() {}

public:
    //Basic constructors
    FilterGLNPasses();

    virtual ImageRAWGL *SetupAuxN(ImageRAWGLVec imgIn, ImageRAWGL *imgOut);

    void InsertFilter(FilterGL *flt);

    Fbo  *getFbo()
    {
        return filters.back()->getFbo();
    }

    //Processing
    ImageRAWGL *Process(ImageRAWGLVec imgIn, ImageRAWGL *imgOut);
};

//Basic constructor
FilterGLNPasses::FilterGLNPasses(): FilterGL()
{
    imgTmp[0] = imgTmp[1] = NULL;
    target = GL_TEXTURE_2D;
}

void FilterGLNPasses::InsertFilter(FilterGL *flt)
{
    if(flt == NULL) {
        return;
    }

    if(flt->filters.size() > 0) {
        for(unsigned int i = 0; i < flt->filters.size(); i++) {
            InsertFilter(flt->filters[i]);
        }
    } else {

#ifdef PIC_DEBUG
        printf("Add Single Filter\n");
#endif
        filters.push_back(flt);
    }
}

ImageRAWGL *FilterGLNPasses::SetupAuxN(ImageRAWGLVec imgIn, ImageRAWGL *imgOut)
{
    if(imgOut == NULL) {
        imgOut = imgIn[0]->AllocateSimilarOneGL();
    }

    if((filters.size() % 2) == 0) {
        imgTmp[1] = imgOut;

        if(imgTmp[0] == NULL) {
            imgTmp[0] = imgOut->AllocateSimilarOneGL();
        }
    } else {
        imgTmp[0] = imgOut;

        if(imgTmp[1] == NULL) {
            imgTmp[1] = imgOut->AllocateSimilarOneGL();
        }
    }

    /*
    if(fbo==NULL)
    	fbo = new Fbo();

    fbo->create(imgOut->width,imgOut->height,imgOut->frames, false, imgOut->getTexture());

    for(unsigned int i=0;i<filters.size();i++)
    	filters[i]->setFbo(fbo);*/

    return imgOut;
}

//Processing
ImageRAWGL *FilterGLNPasses::Process(ImageRAWGLVec imgIn, ImageRAWGL *imgOut)
{
    if(imgIn.size() < 1 || imgIn[0] == NULL) {
        return imgOut;
    }

    //Allocate FBOs
    imgOut = SetupAuxN(imgIn, imgOut);

    filters[0]->ChangePass(0, filters.size());
    filters[0]->Process(imgIn, imgTmp[0]);

    for(unsigned int i = 1; i < filters.size(); i++) {
        filters[i]->ChangePass(i, filters.size());
        imgIn[0] = imgTmp[(i + 1) % 2];
        filters[i]->Process(imgIn, imgTmp[i % 2]);
    }

    return imgOut;
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_NPASSES_HPP */

