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

#ifndef PIC_GL_FILTERING_FILTER_OP_HPP
#define PIC_GL_FILTERING_FILTER_OP_HPP

#include "gl/filtering/filter.hpp"
#include "util/string.hpp"

namespace pic {

class FilterGLOp: public FilterGL
{
protected:
    std::string op;
    float		c0[4], c1[4];
    bool		bTexelFetch;

    void InitShaders();

public:
    //Basic constructor
    FilterGLOp(std::string op, bool bTexelFetch, float *c0, float *c1);
    ~FilterGLOp() {}

    //Update
    void Update(float *c0, float *c1);

    //Processing
    ImageRAWGL *Process(ImageRAWGLVec imgIn, ImageRAWGL *imgOut);

    static FilterGLOp *CreateOpSetZero()
    {
        float val[4] = {0.0f, 0.0f, 0.0f, 1.0f};
        FilterGLOp *filter = new FilterGLOp("C0", true, val, NULL);
        return filter;
    }

    static FilterGLOp *CreateOpIdentity(bool bType)
    {
        FilterGLOp *filter = new FilterGLOp("I0", bType, NULL, NULL);
        return filter;
    }

    static FilterGLOp *CreateOpSegmentation(bool bType, float minVal)
    {
        float tmp[4];
        tmp[0] = tmp[1] = tmp[2] = tmp[3] = minVal;
        FilterGLOp *filter = new FilterGLOp("(I0.x > 0.0)? floor(log(I0) / 2.3026) : C0 ",
                                            bType, tmp, NULL);
        return filter;
    }

    static FilterGLOp *CreateOpAdd(bool bType)
    {
        FilterGLOp *filter = new FilterGLOp("I0 + I1", bType, NULL, NULL);
        return filter;
    }

    static FilterGLOp *CreateOpMulNeg(bool bType)
    {
        FilterGLOp *filter = new FilterGLOp("(vec4(1.0) - I1) * I0", bType, NULL, NULL);
        return filter;
    }

    static FilterGLOp *CreateOpMul(bool bType)
    {
        FilterGLOp *filter = new FilterGLOp("I0 * I1", bType, NULL, NULL);
        return filter;
    }

    static FilterGLOp *CreateOpSub(bool bType)
    {
        FilterGLOp *filter = new FilterGLOp("I0 - I1", bType, NULL, NULL);
        return filter;
    }

    static FilterGLOp *CreateOpDiv(bool bType)
    {
        FilterGLOp *filter = new FilterGLOp("I0 / I1", bType, NULL, NULL);
        return filter;
    }

    static FilterGLOp *CreateOpDivConst(bool bType)
    {
        FilterGLOp *filter = new FilterGLOp("I0 / C0", bType, NULL, NULL);
        return filter;
    }

    //Execute
    static ImageRAWGL *ExecuteADD(std::string nameIn1, std::string nameIn2,
                                  std::string nameOut)
    {
        ImageRAWGL imgIn1(nameIn1);
        imgIn1.generateTextureGL(false);

        ImageRAWGL imgIn2(nameIn2);
        imgIn2.generateTextureGL(false);

        FilterGLOp *flt = FilterGLOp::CreateOpAdd(false);
        ImageRAWGL *imgOut = flt->Process(DoubleGL(&imgIn1, &imgIn2), NULL);
        imgOut->loadToMemory();
        imgOut->Write(nameOut);
        return imgOut;
    }

    static ImageRAWGL *ExecuteIdentity(std::string nameIn1, std::string nameOut)
    {
        ImageRAWGL imgIn1(nameIn1);
        imgIn1.generateTextureGL(false);

        FilterGLOp *flt = FilterGLOp::CreateOpIdentity(false);
        ImageRAWGL *imgOut = flt->Process(SingleGL(&imgIn1), NULL);
        imgOut->loadToMemory();
        imgOut->Write(nameOut);
        return imgOut;
    }
};

FilterGLOp::FilterGLOp(std::string op, bool bTexelFetch = false,
                       float *c0 = NULL, float *c1 = NULL): FilterGL()
{
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

    if(!bTexelFetch) {
        quad = new QuadGL(true);
        vertex_source = QuadGL::getVertexProgramWithTexCoordinates();
    }

    InitShaders();
}

void FilterGLOp::InitShaders()
{
    std::string strOp = "ret = ";
    strOp.append(op);
    strOp.append(";\n");
    int counter;

    //I1
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

    //I2
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
                          out     vec4      f_color; \n
                          in      vec2      v_tex_coord; \n
                          \n
    void main(void) {
        \n
        _COORDINATES_FOR_FETCHING_ \n
        vec4 ret;
        \n
        _PROCESSING_OPERATOR_ \n
        f_color = ret;
        \n
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
    prefix += glw::version("330");

    filteringProgram.setup(prefix, vertex_source, fragment_source);
#ifdef PIC_DEBUG
    printf("[filteringProgram log]\n%s\n", filteringProgram.log().c_str());
#endif
    glw::bind_program(filteringProgram);
    filteringProgram.attribute_source("a_position", 0);

    if(!bTexelFetch) {
        filteringProgram.attribute_source("a_tex_coord",  1);
    }

    filteringProgram.fragment_target("f_color", 0);
    filteringProgram.relink();
    glw::bind_program(filteringProgram);
    filteringProgram.uniform("u_tex_0",  0);
    filteringProgram.uniform("u_tex_1",  1);
    filteringProgram.uniform4("u_val_0", c0);
    filteringProgram.uniform4("u_val_1", c1);
    glw::bind_program(0);
}

//Update
void FilterGLOp::Update(float *c0, float *c1)
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

//Processing
ImageRAWGL *FilterGLOp::Process(ImageRAWGLVec imgIn, ImageRAWGL *imgOut)
{
    if(imgIn[0] == NULL) {
        return imgOut;
    }

    int w = imgIn[0]->width;
    int h = imgIn[0]->height;

    if(imgOut == NULL) {
        imgOut = new ImageRAWGL(1, w, h, imgIn[0]->channels, IMG_GPU);
    }

    if(fbo == NULL) {
        fbo = new Fbo();
    }

    fbo->create(w, h, 1, false, imgOut->getTexture());

    //Rendering
    fbo->bind();
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    //Shaders
    glw::bind_program(filteringProgram);

    //Textures
    for(unsigned int i = 0; i < imgIn.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, imgIn[i]->getTexture());
    }

    quad->Render();

    //Fbo
    fbo->unbind();

    //Shaders
    glw::bind_program(0);

    //Textures
    for(unsigned int i = 0; i < imgIn.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    return imgOut;
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_OP_HPP */

