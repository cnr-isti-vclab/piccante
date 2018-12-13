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

#ifndef PIC_GL_FILTERING_GL_FILTER_WARP_2D_HPP
#define PIC_GL_FILTERING_GL_FILTER_WARP_2D_HPP

#include "../../base.hpp"

#include "../../util/gl/fbo.hpp"

#include "../../util/matrix_3_x_3.hpp"
#include "../../filtering/filter_warp_2d.hpp"
#include "../../gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLWarp2D class
 */
class FilterGLWarp2D: public FilterGL
{
protected:

    int bmin[2], bmax[2];

    Matrix3x3 h, h_inv;
    bool bSameSize, bCentroid;

    /**
     * @brief initShaders
     */
    void initShaders();

public:

    /**
     * @brief FilterGLWarp2D
     * @param h
     * @param bSameSize
     * @param bCentroid
     */
    FilterGLWarp2D();

    /**
     * @brief update
     * @param h
     * @param bSameSize
     * @param bCentroid
     */
    void update(Matrix3x3 h, bool bSameSize, bool bCentroid);

    /**
     * @brief OutputSize
     * @param imgIn
     * @param width
     * @param height
     * @param channels
     * @param frames
     */
    void OutputSize(ImageGLVec imgIn, int &width, int &height, int &channels, int &frames)
    {
        if(!bSameSize) {
            FilterWarp2D::computeBoundingBox(h, bCentroid, imgIn[0]->widthf, imgIn[0]->heightf, bmin, bmax);
        } else {
            bmin[0] = 0;
            bmin[1] = 0;

            bmax[0] = imgIn[0]->width;
            bmax[1] = imgIn[0]->height;
        }

        width = bmax[0] - bmin[0];
        height = bmax[1] - bmin[1];
        channels = imgIn[0]->channels;
        frames = imgIn[0]->frames;
    }

    /**
     * @brief setupAux
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageGL *setupAux(ImageGLVec imgIn, ImageGL *imgOut)
    {       
        imgOut = allocateOutputMemory(imgIn, imgOut, false);

        //update the technique
        float mid[2];

        if(bCentroid) {
            mid[0] = imgIn[0]->widthf  * 0.5f;
            mid[1] = imgIn[0]->heightf * 0.5f;
        } else {
            mid[0] = 0.0f;
            mid[1] = 0.0f;
        }

        technique.bind();
        technique.setUniform2f("mid", mid[0], mid[1]);
        technique.setUniform2f("inv_tSize", 1.0f / imgIn[0]->widthf, 1.0f / imgIn[0]->heightf);
        technique.unbind();

        return imgOut;
    }
};

PIC_INLINE FilterGLWarp2D::FilterGLWarp2D() : FilterGL()
{
    initShaders();

    Matrix3x3 h;
    update(h, false, false);
}

PIC_INLINE void FilterGLWarp2D::update(Matrix3x3 h, bool bSameSize = false, bool bCentroid = false)
{
    this->bSameSize = bSameSize;
    this->bCentroid = bCentroid;

    this->h = h;
    h.inverse(&h_inv);

    technique.bind();
    technique.setUniform3x3("h_inv", h_inv.data, true);
    technique.unbind();
}

PIC_INLINE void FilterGLWarp2D::initShaders()
{
    //fragment program
    fragment_source = MAKE_STRING
                      (
    uniform sampler2D u_tex; \n
    uniform mat3 h_inv; \n
    uniform vec2 mid; \n
    uniform vec2 inv_tSize; \n
    out  vec4 f_color; \n
    \n
    void main(void) {
        vec2 coords   = gl_FragCoord.xy - mid;\n
        vec3 point_proj = h_inv * vec3(coords, 1.0);
        point_proj /= point_proj.z;
        point_proj.xy += mid;
        point_proj.xy *= inv_tSize;
        vec3 color = vec3(0.0);
        if(point_proj.x >= 0.0 && point_proj.x <= 1.0 &&
           point_proj.y >= 0.0 && point_proj.y <= 1.0) {
            color = texture(u_tex, point_proj.xy).xyz;\n
        } \n
        f_color = vec4(color, 1.0); \n
    }
    );

    technique.initStandard("330", vertex_source, fragment_source, "FilterGLWarp2D");

    technique.bind();
    technique.setUniform1i("u_tex", 0);
    technique.unbind();
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_WARP_2D_HPP */

