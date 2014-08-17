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

#ifndef PIC_GL_FILTERING_FILTER_REDUX_HPP
#define PIC_GL_FILTERING_FILTER_REDUX_HPP

#include "gl/filtering/filter.hpp"

namespace pic {

class FilterGLRedux: public FilterGL
{
protected:
    std::string reduxOperation;

    void InitShaders();

public:
    /**
     * @brief FilterGLRedux
     * @param reduxOperation
     */
    FilterGLRedux(std::string reduxOperation);

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageRAWGL *Process(ImageRAWGLVec imgIn, ImageRAWGL *imgOut);

    /**
     * @brief CreateMean
     * @return
     */
    static FilterGLRedux *CreateMean()
    {
        FilterGLRedux *filter = new
        FilterGLRedux("color = (color00+color10+color01+color11)*0.25;");
        return filter;
    }

    /**
     * @brief CreateMax
     * @return
     */
    static FilterGLRedux *CreateMax()
    {
        FilterGLRedux *filter = new
        FilterGLRedux("color = max(color00,color10);\n color = max(color, color01);\n color = max(color, color11);\n");
        return filter;
    }

    /**
     * @brief CreateMin
     * @return
     */
    static FilterGLRedux *CreateMin()
    {
        FilterGLRedux *filter = new
        FilterGLRedux("color = min(color00,color10);\n color = min(color, color01);\n color = min(color, color11);\n");
        return filter;
    }

    /**
     * @brief CreateMinPos
     * @return
     */
    static FilterGLRedux *CreateMinPos()
    {
        FilterGLRedux *filter = new
        FilterGLRedux("vec4 maxVal = vec4(1e-6); if(color00.x>0.0f) color = color00; if(color01.x>0.0f) color = min(color,color01); if(color10.x>0.0f) color = min(color,color10); if(color11.x>0.0f) color = min(color,color11);\n");
        return filter;
    }

    /**
     * @brief CreateCheck
     * @return
     */
    static FilterGLRedux *CreateCheck()
    {
        FilterGLRedux *filter = new
        FilterGLRedux("vec4 sum = color00 + color01 + color10 + color11; color = sum.x<0.5? vec4(0.0) : sum; color = ((sum.x>0.5)&&sum.x<3.5)? vec4(10.0) : color; color = ((sum.x>3.5)&&sum.x<4.5)? vec4(1.0) : color; color = sum.x>4.5 ? vec4(10.0) : color;\n");
        return filter;
    }

    /**
     * @brief EvenOdd
     * @param val
     * @return
     */
    static int EvenOdd(int val)
    {
        if((val % 2) == 0) {
            val = val >> 1;
        } else {
            val = (val >> 1) + 1;
        }

        return val;
    }

    /**
     * @brief CreateData
     * @param width
     * @param height
     * @param channels
     * @param minSize
     * @return
     */
    static ImageRAWGLVec CreateData(int width, int height, int channels ,
                                         int minSize = 2)
    {
        int checkSize = EvenOdd(MIN(width, height));

        ImageRAWGLVec stack;

        while(checkSize > minSize) {
            width  = EvenOdd(width);
            height = EvenOdd(height);

            ImageRAWGL *tmpImg = new ImageRAWGL(1, width, height, channels, IMG_GPU, GL_TEXTURE_2D);
            stack.push_back(tmpImg);

            checkSize = MIN(width, height);
        }

        return stack;
    }

    /**
     * @brief Redux
     * @param imgIn
     * @param stack
     * @return
     */
    ImageRAWGL *Redux(ImageRAWGL *imgIn, ImageRAWGLVec stack)
    {
        ImageRAWGL *imageFlt = imgIn;

        for(unsigned int i = 0; i < stack.size(); i++) {
            Process(SingleGL(imageFlt), stack[i]);
            imageFlt = stack[i];
        }

        return stack[stack.size() - 1];
    }

    /**
     * @brief ExecuteReduxMean
     * @param nameIn
     * @param nameOut
     * @return
     */
    static ImageRAWGL *ExecuteReduxMean(std::string nameIn, std::string nameOut)
    {
        ImageRAWGL imgIn(nameIn);
        imgIn.generateTextureGL(false, GL_TEXTURE_2D);

        FilterGLRedux *filter = CreateMean();

        ImageRAWGLVec stack = CreateData(imgIn.width, imgIn.height, imgIn.channels,
                                              1);

        ImageRAWGL *out = filter->Redux(&imgIn, stack);

        out->loadToMemory();
        out->Write(nameOut);

        return out;
    }

    /**
     * @brief ExecuteReduxMax
     * @param nameIn
     * @param nameOut
     * @return
     */
    static ImageRAWGL *ExecuteReduxMax(std::string nameIn, std::string nameOut)
    {
        ImageRAWGL imgIn(nameIn);
        imgIn.generateTextureGL(false, GL_TEXTURE_2D);

        FilterGLRedux *filter = CreateMax();

        ImageRAWGLVec stack = CreateData(imgIn.width, imgIn.height, imgIn.channels,
                                              1);

        ImageRAWGL *out = filter->Redux(&imgIn, stack);

        out->loadToMemory();
        out->Write(nameOut);

        return out;
    }
};

FilterGLRedux::FilterGLRedux(std::string reduxOperation): FilterGL()
{
    this->reduxOperation = reduxOperation;
    InitShaders();
}

void FilterGLRedux::InitShaders()
{
    fragment_source = GLW_STRINGFY
                      (

                          uniform sampler2D u_tex; \n
                          out     vec4      f_color; \n

    void main(void) {
        \n
        ivec2 texSize = textureSize(u_tex, 0);
        ivec2 coords  = ivec2(gl_FragCoord.xy) * 2;
        \n
        vec4  color00 = texelFetch(u_tex, coords,            0);
        \n
        vec4  color10 = texelFetch(u_tex, coords + ivec2(1, 0), 0);
        \n
        vec4  color01 = texelFetch(u_tex, coords + ivec2(0, 1), 0);
        \n
        vec4  color11 = texelFetch(u_tex, coords + ivec2(1, 1), 0);
        \n
        vec4  color;
        \n
        ___REDUX_OPERATION___ \n
        f_color = ((texSize - coords) == ivec2(0, 0)) ? color00 : color;
        f_color = vec4(color.xyz, 1.0);
        \n
    }
                      );

    size_t processing_found = fragment_source.find("___REDUX_OPERATION___");
    fragment_source.replace(processing_found, 21, reduxOperation);

    filteringProgram.setup(glw::version("330"), vertex_source, fragment_source);
#ifdef PIC_DEBUG
    printf("[filteringProgram log]\n%s\n", filteringProgram.log().c_str());
#endif

    glw::bind_program(filteringProgram);
    filteringProgram.attribute_source("a_position", 0);
    filteringProgram.fragment_target("f_color",    0);
    filteringProgram.relink();
    filteringProgram.uniform("u_tex",      0);
    glw::bind_program(0);
}

ImageRAWGL *FilterGLRedux::Process(ImageRAWGLVec imgIn, ImageRAWGL *imgOut)
{
    if(imgIn[0] == NULL) {
        return NULL;
    }

    if(imgIn.size() != 1) {
        return imgOut;
    }

    int w = EvenOdd(imgIn[0]->width);
    int h = EvenOdd(imgIn[0]->height);
    int f = imgIn[0]->frames;

    if(imgOut == NULL) {
        imgOut = new ImageRAWGL(f, w, h, 4, IMG_GPU, GL_TEXTURE_2D);
    }

    if(imgOut->width < 1 || imgOut->height < 1) {
        return imgOut;
    }

    //Fbo
    if(fbo == NULL) {
        fbo = new Fbo();
    }

    fbo->create(w, h, f, false, imgOut->getTexture());

    //Rendering
    fbo->bind();
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    //Shaders
    glw::bind_program(filteringProgram);

    //Textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, imgIn[0]->getTexture());

    //Rendering aligned quad
    quad->Render();

    //Fbo
    fbo->unbind();

    //Shaders
    glw::bind_program(0);

    //Textures
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    return imgOut;
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_REDUX_HPP */

