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

#ifndef PIC_UTIL_GL_BUFFER_OP_HPP
#define PIC_UTIL_GL_BUFFER_OP_HPP

#include "util/string.hpp"
#include "util/gl/quad.hpp"
#include "util/gl/fbo.hpp"

namespace pic {

/**
 * @brief The BufferOpGL class
 */
class BufferOpGL
{
protected:

    //FBO
    Fbo *fbo;

    //Quad
    QuadGL *quad;

    //Shaders
    glw::program filteringProgram;
    GLenum target;

    std::string op;
    float		c0[4], c1[4];
    bool		bTexelFetch;

    /**
     * @brief InitShaders
     */
    void InitShaders();

public:

    std::string vertex_source, geometry_source, fragment_source;

    /**
     * @brief BufferOpGL
     * @param op
     * @param bTexelFetch
     * @param c0
     * @param c1
     */
    BufferOpGL(std::string op, bool bTexelFetch, float *c0, float *c1);

    /**
     * @brief Update
     * @param c0
     * @param c1
     */
    void Update(float *c0, float *c1);

    /**
     * @brief Update
     * @param c0
     * @param c1
     */
    void Update(float c0, float c1);

    /**
     * @brief Process
     * @param tex0
     * @param tex1
     * @param texOut
     * @param width
     * @param height
     */
    void Process(GLuint tex0, GLuint tex1, GLuint texOut, int width, int height);

};

BufferOpGL::BufferOpGL(std::string op, bool bTexelFetch = false, float *c0 = NULL, float *c1 = NULL)
{
    fbo = NULL;

    quad = NULL;

    target = GL_TEXTURE_2D;

    if(c0 != NULL) {
        memcpy(this->c0, c0, 4 * sizeof(float));
    } else {
        for(int i = 0; i < 4; i++) {
            this->c0[i] = 1.0f;
        }
    }

    if(c1 != NULL) {
        memcpy(this->c1, c1, 4 * sizeof(float));
    } else {
        for(int i = 0; i < 4; i++) {
            this->c0[i] = 1.0f;
        }
    }

    this->op = op;
    this->bTexelFetch = bTexelFetch;

    if(bTexelFetch) {
        vertex_source = QuadGL::getVertexProgramV3();
        quad = new QuadGL(false);
    } else {
        vertex_source = QuadGL::getVertexProgramWithTexCoordinates();
        quad = new QuadGL(true);
    }

    InitShaders();
}

void BufferOpGL::InitShaders()
{
    std::string strOp = "vec4 ret = ";
    strOp.append(op);
    strOp.append(";\n");
    int counter;

    //I0x
    counter = countSubString(strOp, "I0x");

    if(counter == 1) {
        size_t I_found = strOp.find("I0x");

        if(I_found != std::string::npos) {
            if(bTexelFetch) {
                strOp.replace(I_found, 3, "texelFetch(u_tex_0, coords, 0).xxxx");
            } else {
                strOp.replace(I_found, 3, "texture(u_tex_0, coords).xxxx");
            }
        }

    } else {
        if(counter > 1) {
            if(bTexelFetch) {
                strOp = "vec4 tmp0x = texelFetch(u_tex_0, coords, 0);\n" + strOp;
             } else {
                strOp = "vec4 tmp0x = texture(u_tex_0, coords);\n" + strOp;
             }
             strOp = stdStringRepAll(strOp, "I0x", "tmp0x");
        }
    }

    //I1x
    counter = countSubString(strOp, "I1x");

    if(counter == 1) {
        size_t I_found = strOp.find("I1x");

        if(I_found != std::string::npos) {
            if(bTexelFetch) {
                strOp.replace(I_found, 3, "texelFetch(u_tex_1, coords, 0).xxxx");
            } else {
                strOp.replace(I_found, 3, "texture(u_tex_1, coords).xxxx");
            }
        }

    } else {
        if(counter > 1) {
            if(bTexelFetch) {
                strOp = "vec4 tmp1x = texelFetch(u_tex_1, coords, 0);\n" + strOp;
             } else {
                strOp = "vec4 tmp1x = texture(u_tex_1, coords);\n" + strOp;
             }
             strOp = stdStringRepAll(strOp, "I1x", "tmp1x");
        }
    }

    //I0
    counter = countSubString(strOp, "I0");

    if(counter == 1) {
        size_t I_found = strOp.find("I0");

        if(I_found != std::string::npos) {
            if(bTexelFetch) {
                strOp.replace(I_found, 2, "texelFetch(u_tex_0, coords, 0)");
            } else {
                strOp.replace(I_found, 2, "texture(u_tex_0, coords)");
            }
        }
    } else {
        if(counter > 1) {
            if(bTexelFetch) {
                strOp = "vec4 tmp0 = texelFetch(u_tex_0, coords, 0);\n" + strOp;
            } else {
                strOp = "vec4 tmp0 = texture(u_tex_0, coords);\n" + strOp;
            }

            strOp = stdStringRepAll(strOp, "I0", "tmp0");
        }
    }

    //I1
    counter = countSubString(strOp, "I1");

    if(counter == 1) {
        size_t I_found = strOp.find("I1");

        if(I_found != std::string::npos) {
            if(bTexelFetch) {
                strOp.replace(I_found, 2, "texelFetch(u_tex_1, coords, 0)");
            } else {
                strOp.replace(I_found, 2, "texture(u_tex_1, coords)");
            }
        }
    } else {
        if(counter > 1) {
            if(bTexelFetch) {
                strOp = "vec4 tmp1 = texelFetch(u_tex_1, coords, 0);\n" + strOp;
            } else {
                strOp = "vec4 tmp1 = texture(u_tex_1, coords);\n" + strOp;
            }

            strOp = stdStringRepAll(strOp, "I1", "tmp1");
        }
    }

    //C1 and C2
    strOp = stdStringRepAll(strOp, "C0", "u_val_0");
    strOp = stdStringRepAll(strOp, "C1", "u_val_1");

    fragment_source = GLW_STRINGFY
                      (
                          uniform sampler2D u_tex_0; \n
                          uniform sampler2D u_tex_1; \n
                          uniform vec4      u_val_0; \n
                          uniform vec4      u_val_1; \n
                          in      vec2      v_tex_coord; \n
                          out     vec4      f_color; \n
                          \n
    void main(void) { \n
        _COORDINATES_FOR_FETCHING_ \n
        _PROCESSING_OPERATOR_ \n
        f_color = ret; \n
    }
                      );

    if(bTexelFetch) {
        size_t processing_found = fragment_source.find("_COORDINATES_FOR_FETCHING_");
        fragment_source.replace(processing_found, 27,
                                "ivec2 coords = ivec2(gl_FragCoord.xy);\n");
    } else {
        size_t processing_found = fragment_source.find("_COORDINATES_FOR_FETCHING_");
        fragment_source.replace(processing_found, 27,
                                "vec2 coords = v_tex_coord.xy;\n");
    }

    size_t processing_found = fragment_source.find("_PROCESSING_OPERATOR_");
    fragment_source.replace(processing_found, 21, strOp);

    std::string prefix;
    prefix += glw::version("400");

    filteringProgram.setup(prefix, vertex_source, fragment_source);

#ifdef PIC_DEBUG
    printf("[BufferOp log]\n%s\n", filteringProgram.log().c_str());
#endif

    glw::bind_program(filteringProgram);
    filteringProgram.attribute_source("a_position", 0);

    if(!bTexelFetch) {
        filteringProgram.attribute_source("a_tex_coord",  1);
    }

    filteringProgram.fragment_target("f_color", 0);
    filteringProgram.relink();
    glw::bind_program(0);

    glw::bind_program(filteringProgram);
    filteringProgram.uniform("u_tex_0",  0);
    filteringProgram.uniform("u_tex_1",  1);
    filteringProgram.uniform4("u_val_0", c0);
    filteringProgram.uniform4("u_val_1", c1);
    glw::bind_program(0);
}

void BufferOpGL::Update(float *c0, float *c1)
{
    if(c0 != NULL) {
        for(int i = 0; i < 4; i++) {
            this->c0[i] = c0[i];
        }
    }

    if(c1 != NULL) {
        for(int i = 0; i < 4; i++) {
            this->c1[i] = c1[i];
        }
    }

    glw::bind_program(filteringProgram);
    filteringProgram.uniform("u_tex_0",  0);
    filteringProgram.uniform("u_tex_1",  1);
    filteringProgram.uniform4("u_val_0", this->c0);
    filteringProgram.uniform4("u_val_1", this->c1);
    glw::bind_program(0);
}

void BufferOpGL::Update(float c0 = 0.0f, float c1 = 0.0f)
{
    for(int i = 0; i < 4; i++) {
        this->c0[i] = c0;
    }

    for(int i = 0; i < 4; i++) {
        this->c1[i] = c1;
    }

    glw::bind_program(filteringProgram);
    filteringProgram.uniform("u_tex_0",  0);
    filteringProgram.uniform("u_tex_1",  1);
    filteringProgram.uniform4("u_val_0", this->c0);
    filteringProgram.uniform4("u_val_1", this->c1);
    glw::bind_program(0);
}

void BufferOpGL::Process(GLuint tex0, GLuint tex1, GLuint texOut, int width, int height)
{
    if(texOut == 0) {
        #ifdef PIC_DEBUG
            printf("BufferOpGL::Process: the output texture, texOut, is empty.\n");
        #endif
        return;
    }

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
    glBindTexture(GL_TEXTURE_2D, tex0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex1);

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
}

} // end namespace pic

#endif /* PIC_UTIL_GL_BUFFER_OP_HPP */
