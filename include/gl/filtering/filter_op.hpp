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

#ifndef PIC_GL_FILTERING_FILTER_OP_HPP
#define PIC_GL_FILTERING_FILTER_OP_HPP

#include "../../base.hpp"

#include "../../gl/filtering/filter.hpp"
#include "../../util/string.hpp"

namespace pic {

class FilterGLOp: public FilterGL
{
protected:
    std::string op;
    float c0[4], c1[4];
    bool bTexelFetch;

    void initShaders();

public:

    /**
     * @brief FilterGLOp
     * @param op
     * @param bTexelFetch
     * @param c0
     * @param c1
     */
    FilterGLOp(std::string op, bool bTexelFetch, float *c0, float *c1);

    /**
     * @brief update
     * @param c0
     * @param c1
     */
    void update(float *c0, float *c1);

    /**
     * @brief CreateOpSetZero
     * @return
     */
    static FilterGLOp *CreateOpSetZero()
    {
        float val[4] = {0.0f, 0.0f, 0.0f, 1.0f};
        FilterGLOp *filter = new FilterGLOp("C0", true, val, NULL);
        return filter;
    }

    /**
     * @brief CreateOpSetOne
     * @return
     */
    static FilterGLOp *CreateOpSetOne()
    {
        float val[4] = {1.0f, 1.0f, 1.0f, 1.0f};
        FilterGLOp *filter = new FilterGLOp("C0", true, val, NULL);
        return filter;
    }

    /**
     * @brief CreateOpIdentity
     * @param bType
     * @return
     */
    static FilterGLOp *CreateOpIdentity(bool bType)
    {
        FilterGLOp *filter = new FilterGLOp("I0", bType, NULL, NULL);
        return filter;
    }

    /**
     * @brief CreateOpSegmentation
     * @param bType
     * @param minVal
     * @return
     */
    static FilterGLOp *CreateOpSegmentation(bool bType, float minVal)
    {
        float tmp[4];
        tmp[0] = tmp[1] = tmp[2] = tmp[3] = minVal;
        FilterGLOp *filter = new FilterGLOp("(I0.x > 0.0) ? floor(log(I0) / 2.3026) : C0 ",
                                            bType, tmp, NULL);
        return filter;
    }

    /**
     * @brief CreateOpAdd
     * @param bType
     * @return
     */
    static FilterGLOp *CreateOpAdd(bool bType)
    {
        FilterGLOp *filter = new FilterGLOp("I0 + I1", bType, NULL, NULL);
        return filter;
    }

    /**
     * @brief CreateOpMulNeg
     * @param bType
     * @return
     */
    static FilterGLOp *CreateOpMulNeg(bool bType)
    {
        FilterGLOp *filter = new FilterGLOp("(vec4(1.0) - I1) * I0", bType, NULL, NULL);
        return filter;
    }

    /**
     * @brief CreateOpMul
     * @param bType
     * @return
     */
    static FilterGLOp *CreateOpMul(bool bType)
    {
        FilterGLOp *filter = new FilterGLOp("I0 * I1", bType, NULL, NULL);
        return filter;
    }

    /**
     * @brief CreateOpSub
     * @param bType
     * @return
     */
    static FilterGLOp *CreateOpSub(bool bType)
    {
        FilterGLOp *filter = new FilterGLOp("I0 - I1", bType, NULL, NULL);
        return filter;
    }

    /**
     * @brief CreateOpDiv
     * @param bType
     * @return
     */
    static FilterGLOp *CreateOpDiv(bool bType)
    {
        FilterGLOp *filter = new FilterGLOp("I0 / I1", bType, NULL, NULL);
        return filter;
    }

    /**
     * @brief CreateOpDivConst
     * @param bType
     * @return
     */
    static FilterGLOp *CreateOpDivConst(bool bType)
    {
        FilterGLOp *filter = new FilterGLOp("I0 / C0", bType, NULL, NULL);
        return filter;
    }
};

PIC_INLINE FilterGLOp::FilterGLOp(std::string op, bool bTexelFetch = false,
                       float *c0 = NULL, float *c1 = NULL): FilterGL()
{
    if(c0 != NULL) {
        memcpy(this->c0, c0, 4 * sizeof(float));
    } else {
        Arrayf::assign(1.0f, this->c0, 4);
    }

    if(c1 != NULL) {
        memcpy(this->c1, c1, 4 * sizeof(float));
    } else {
        Arrayf::assign(1.0f, this->c1, 4);
    }

    this->op = op;
    this->bTexelFetch = bTexelFetch;

    if(!bTexelFetch) {

        if(quad != NULL) {
            delete quad;
        }

        quad = new QuadGL(true);
        vertex_source = QuadGL::getVertexProgramWithTexCoordinates();
    }

    initShaders();
}

PIC_INLINE void FilterGLOp::initShaders()
{
    std::string strOp = "ret = ";
    strOp.append(op);
    strOp.append(";\n");
    int counter;

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

    fragment_source = MAKE_STRING
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

    technique.init("330", vertex_source, fragment_source);

#ifdef PIC_DEBUG
    technique.printLog("FilterOp");
#endif

    technique.bind();
    technique.setAttributeIndex("a_position", 0);

    if(!bTexelFetch) {
        technique.setAttributeIndex("a_tex_coord",  1);
    }

    technique.setOutputFragmentShaderIndex("f_color", 0);
    technique.link();
    technique.unbind();

    technique.bind();
    technique.setUniform1i("u_tex_0",  0);
    technique.setUniform1i("u_tex_1",  1);
    technique.setUniform4fv("u_val_0", c0);
    technique.setUniform4fv("u_val_1", c1);
    technique.unbind();
}

PIC_INLINE void FilterGLOp::update(float *c0, float *c1)
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

    technique.bind();
    technique.setUniform1i("u_tex_0",  0);
    technique.setUniform1i("u_tex_1",  1);
    technique.setUniform4fv("u_val_0", this->c0);
    technique.setUniform4fv("u_val_1", this->c1);
    technique.unbind();
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_OP_HPP */

