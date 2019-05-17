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

#include "base.hpp"

#include "util/gl/technique.hpp"
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

    //quad
    QuadGL *quad;

    bool bDomainTransform;
    int  counter;

    //shaders
    std::string vertex_source, geometry_source, fragment_source, fragment_source_domain_transform;
    TechniqueGL techinques[2];

    std::string reduxOperation;

    /**
     * @brief initShaders
     */
    void initShaders();

public:
    /**
     * @brief ReduxGL
     * @param reduxOperation
     */
    ReduxGL(std::string reduxOperation, bool bDomainTransform);

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
        if(stack.empty() || (texIn == 0)) {
            return 0;
        }

        GLuint texFlt = texIn;

        for(unsigned int i = 0; i < stack.size(); i++) {
            counter = (bDomainTransform && (i == 0)) ? 1 : 0;

            width  = divideByTwoWithEvenDividend(width);
            height = divideByTwoWithEvenDividend(height);

            Process(texFlt, width, height, channels, stack[i]);
            texFlt = stack[i];
        }

        return texFlt;
    }

    /**
     * @brief createMean
     * @return
     */
    static ReduxGL *createMean()
    {
        ReduxGL *filter = new
        ReduxGL("color = (color00 + color10 + color01 + color11) / 4.0;", false);
        return filter;
    }

    /**
     * @brief createSum
     * @return
     */
    static ReduxGL *createSum()
    {
        ReduxGL *filter = new
        ReduxGL("color = color00 + color10 + color01 + color11;", false);
        return filter;
    }

    /**
     * @brief createLogMean
     * @return
     */
    static ReduxGL *createLogMean()
    {
        ReduxGL *filter = new
        ReduxGL("color = (color00 + color10 + color01 + color11) / 4.0;", true);
        return filter;
    }

    /**
     * @brief createMin
     * @return
     */
    static ReduxGL *createMin()
    {
        ReduxGL *filter = new
        ReduxGL("color = min(color00, color10);\n "
                "color = min(color, color01);\n "
                "color = min(color, color11);\n", false);
        return filter;
    }

    /**
     * @brief createMax
     * @return
     */
    static ReduxGL *createMax()
    {
        ReduxGL *filter = new
        ReduxGL("color = max(color00, color10);\n "
                "color = max(color, color01);\n "
                "color = max(color, color11);\n", false);
        return filter;
    }

    /**
     * @brief createMinPos
     * @return
     */
    static ReduxGL *createMinPos()
    {
        ReduxGL *filter = new
        ReduxGL("vec4 maxVal = vec4(1e-6); "
                "if(color00.x>0.0f) color = color00; "
                "if(color01.x>0.0f) color = min(color,color01); "
                "if(color10.x>0.0f) color = min(color,color10); "
                "if(color11.x>0.0f) color = min(color,color11);\n", false);
        return filter;
    }

    /**
     * @brief createCheck
     * @return
     */
    static ReduxGL *createCheck()
    {
        ReduxGL *filter = new
        ReduxGL("vec4 sum = color00 + color01 + color10 + color11; "
                "color = sum.x < 0.5? vec4(0.0) : sum; "
                "color = ((sum.x > 0.5) && (sum.x < 3.5))? vec4(10.0) : color; "
                "color = ((sum.x > 3.5) && (sum.x < 4.5))? vec4(1.0) : color; "
                "color = sum.x > 4.5 ? vec4(10.0) : color;\n", false);
        return filter;
    }

    /**
     * @brief divideByTwoWithEvenDividend if x is even it computes x / 2 otherwise (x + 1) / 2.
     * @param x is an input value.
     * @return If x is even it returns x/2; otherwise it returns (x + 1) / 2.
     */
    static int divideByTwoWithEvenDividend(int x)
    {
        if((x % 2) != 0) {
            x++;
        }

        return (x >> 1);
    }

    /**
     * @brief allocateReduxData allocates a pyramid for computing the Redux operator.
     * @param width
     * @param height
     * @param channels
     * @param minSize
     * @return
     */
    static void allocateReduxData(int width, int height, int channels,
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

PIC_INLINE ReduxGL::ReduxGL(std::string reduxOperation, bool bDomainTransform)
{
    fbo = NULL;

    quad = NULL;

    this->counter = 0;
    this->bDomainTransform = bDomainTransform;

    quad = new QuadGL(false);

    //get a vertex program for screen aligned quad
    vertex_source = QuadGL::getVertexProgramV3();

    this->reduxOperation = reduxOperation;
    initShaders();
}

PIC_INLINE ReduxGL::~ReduxGL()
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

PIC_INLINE void ReduxGL::initShaders()
{
    fragment_source = MAKE_STRING
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
        f_color = vec4(color.xyz, 1.0);
        \n
    }
                      );
//    f_color = ((texSize - coords) == ivec2(0, 0)) ? color00 : color;

    fragment_source_domain_transform = fragment_source;

    size_t processing_found = fragment_source.find("___REDUX_OPERATION___");
    fragment_source.replace(processing_found, 21, reduxOperation);

    techinques[0].initStandard("330", vertex_source, fragment_source, "ReduxGL");

    techinques[0].bind();
    techinques[0].setUniform1i("u_tex", 0);
    techinques[0].unbind();

    if(bDomainTransform) {
        size_t processing_found = fragment_source_domain_transform.find("___REDUX_OPERATION___");
        std::string domain_transform = "float eps = 1e-6; "
                                       "color00 = log(color00 + eps); "
                                       "color01 = log(color01 + eps); "
                                       "color10 = log(color10 + eps) ; "
                                       "color11 = log(color11 + eps);";
        domain_transform += reduxOperation;
        fragment_source_domain_transform.replace(processing_found, 21, domain_transform);

        techinques[1].initStandard("330", vertex_source, fragment_source_domain_transform, "ReduxGL");

        techinques[1].bind();
        techinques[1].setUniform1i("u_tex", 0);
        techinques[1].unbind();
    }
}

PIC_INLINE GLuint ReduxGL::Process(GLuint texIn, int width, int height, int channels, GLuint texOut)
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

    //check the fbo
    if(fbo == NULL) {
        fbo = new Fbo();
    }

    fbo->create(width, height, 1, false, texOut);

    //bind the fbo
    fbo->bind();
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);

    //bind shaders
    techinques[counter].bind();

    //bind textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texIn);

    //render an aligned quad
    quad->Render();

    //unbind the fbo
    fbo->unbind();

    //unbind shaders
    techinques[counter].unbind();

    //unbind textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texOut;
}

} // end namespace pic

#endif /* PIC_UTIL_GL_REDUX_HPP */

