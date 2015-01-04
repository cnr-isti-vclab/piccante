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

#ifndef PIC_UTIL_GL_REDUX_HPP
#define PIC_UTIL_GL_REDUX_HPP

#include "util/gl/fbo.hpp"
#include "util/gl/quad.hpp"

namespace pic {

/**
 * @brief The ReduxGL class
 */
class ReduxGL
{
protected:
    //FBO
    Fbo *fbo;

    //Quad
    QuadGL *quad;

    //Shaders
    std::string vertex_source, geometry_source, fragment_source;
    glw::program filteringProgram;

    std::string reduxOperation;

    /**
     * @brief InitShaders
     */
    void InitShaders();

public:
    /**
     * @brief ReduxGL
     * @param reduxOperation
     */
    ReduxGL(std::string reduxOperation);

    ~ReduxGL();

    /**
     * @brief Process
     * @param texIn
     * @param width
     * @param height
     * @param channels
     * @param texOut
     * @return
     */
    GLuint Process(GLuint texIn, int width, int height, int channels, GLuint texOut);

    /**
     * @brief Redux
     * @param texIn
     * @param width
     * @param height
     * @param channels
     * @param stack
     * @return
     */
    GLuint Redux(GLuint texIn, int width, int height, int channels, std::vector<GLuint> &stack)
    {
        GLuint texFlt = texIn;

        for(unsigned int i = 0; i < stack.size(); i++) {
            width  = divideByTwoWithEvenDividend(width);
            height = divideByTwoWithEvenDividend(height);

            Process(texFlt, width, height, channels, stack[i]);
            texFlt = stack[i];
        }

        return stack[stack.size() - 1];
    }

    /**
     * @brief CreateMean
     * @return
     */
    static ReduxGL *CreateMean()
    {
        ReduxGL *filter = new
        ReduxGL("color = (color00 + color10 + color01 + color11) / 4.0;");
        return filter;
    }

    /**
     * @brief CreateMax
     * @return
     */
    static ReduxGL *CreateMax()
    {
        ReduxGL *filter = new
        ReduxGL("color = max(color00, color10);\n color = max(color, color01);\n color = max(color, color11);\n");
        return filter;
    }

    /**
     * @brief CreateMin
     * @return
     */
    static ReduxGL *CreateMin()
    {
        ReduxGL *filter = new
        ReduxGL("color = min(color00, color10);\n color = min(color, color01);\n color = min(color, color11);\n");
        return filter;
    }

    /**
     * @brief CreateMinPos
     * @return
     */
    static ReduxGL *CreateMinPos()
    {
        ReduxGL *filter = new
        ReduxGL("vec4 maxVal = vec4(1e-6); if(color00.x>0.0f) color = color00; if(color01.x>0.0f) color = min(color,color01); if(color10.x>0.0f) color = min(color,color10); if(color11.x>0.0f) color = min(color,color11);\n");
        return filter;
    }

    /**
     * @brief CreateCheck
     * @return
     */
    static ReduxGL *CreateCheck()
    {
        ReduxGL *filter = new
        ReduxGL("vec4 sum = color00 + color01 + color10 + color11; color = sum.x<0.5? vec4(0.0) : sum; color = ((sum.x>0.5)&&sum.x<3.5)? vec4(10.0) : color; color = ((sum.x>3.5)&&sum.x<4.5)? vec4(1.0) : color; color = sum.x>4.5 ? vec4(10.0) : color;\n");
        return filter;
    }

    /**
     * @brief divideByTwoWithEvenDividend if x is even it computes x/2 otherwise (x+1)/2.
     * @param x is an input value.
     * @return If x is even it returns x/2; otherwise it returns (x+1)/2.
     */
    static int divideByTwoWithEvenDividend(int x)
    {
        if((x % 2) != 0) {
            x++;
        }

        return (x >> 1);
    }

    /**
     * @brief AllocateReduxData allocates a pyramid for computing the Redux operator.
     * @param width
     * @param height
     * @param channels
     * @param minSize
     * @return
     */
    static void AllocateReduxData(int width, int height, int channels,
                           std::vector<GLuint> &stack, int minSize = 2)
    {
        int checkSize = divideByTwoWithEvenDividend(MIN(width, height));

        stack.clear();

        if(minSize < 2) {
            minSize = 2;
        }

        while(checkSize >= minSize) {
            width  = divideByTwoWithEvenDividend(width);
            height = divideByTwoWithEvenDividend(height);

            stack.push_back(generateTexture2DGL(width, height, channels));

            checkSize = MIN(width, height);
        }
    }

};

ReduxGL::ReduxGL(std::string reduxOperation)
{
    fbo = NULL;

    quad = NULL;

    quad = new QuadGL(false);

    //getting a vertex program for screen aligned quad
    vertex_source = QuadGL::getVertexProgramV3();

    this->reduxOperation = reduxOperation;
    InitShaders();
}

ReduxGL::~ReduxGL()
{
    if(quad != NULL) {
        delete quad;
        quad = NULL;
    }

    if(fbo != NULL) {
        delete fbo;
        fbo = NULL;
    }
}

void ReduxGL::InitShaders()
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
        vec4  color00 = texelFetch(u_tex, coords               ,0);
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
    printf("[ReduxGL log]\n%s\n", filteringProgram.log().c_str());
#endif

    glw::bind_program(filteringProgram);
    filteringProgram.attribute_source("a_position", 0);
    filteringProgram.fragment_target("f_color", 0);
    filteringProgram.relink();
    filteringProgram.uniform("u_tex", 0);
    glw::bind_program(0);
}

GLuint ReduxGL::Process(GLuint texIn, int width, int height, int channels, GLuint texOut)
{

    if(texIn == 0) {
        return texOut;
    }

    if(texOut == 0) {
        texOut = generateTexture2DGL(width, height, channels);
    }

    if(width < 1 || height < 1) {
        return texOut;
    }

    //Fbo
    if(fbo == NULL) {
        fbo = new Fbo();
    }

    fbo->create(width, height, 1, false, texOut);

    //Rendering
    fbo->bind();
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);

    //Shaders
    glw::bind_program(filteringProgram);

    //Textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texIn);

    //Rendering aligned quad
    quad->Render();

    //Fbo
    fbo->unbind();

    //Shaders
    glw::bind_program(0);

    //Textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texOut;
}

} // end namespace pic

#endif /* PIC_UTIL_GL_REDUX_HPP */

