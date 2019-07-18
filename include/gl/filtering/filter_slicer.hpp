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

#ifndef PIC_GL_FILTERING_FILTER_SLICER_HPP
#define PIC_GL_FILTERING_FILTER_SLICER_HPP

#include "../../base.hpp"

#include "../../gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLSlicer class
 */
class FilterGLSlicer: public FilterGL
{
protected:

    void initShaders();
    void FragmentShader();

    float s_S, s_R, mul_E;

public:
    /**
     * @brief FilterGLSlicer
     * @param s_S
     * @param s_R
     */
    FilterGLSlicer(float s_S, float s_R);

    ~FilterGLSlicer();

    /**
     * @brief update
     * @param s_S
     * @param s_R
     */
    void update(float s_S, float s_R);
};

PIC_INLINE FilterGLSlicer::FilterGLSlicer(float s_S, float s_R): FilterGL()
{
    this->s_S = s_S;
    this->s_R = s_R;

    FragmentShader();
    initShaders();
}

PIC_INLINE FilterGLSlicer::~FilterGLSlicer()
{
    release();
}

PIC_INLINE void FilterGLSlicer::FragmentShader()
{
    fragment_source = MAKE_STRING
                      (
                          uniform sampler2D	u_tex;
                          uniform sampler3D	u_grid;
                          uniform float		mul_E;
                          uniform float		s_S;

                          out     vec4       f_color;

    void main(void) {
        //Fetch texture color
        ivec2 coordsFrag = ivec2(gl_FragCoord.xy);
        vec4 colRef = texelFetch(u_tex, coordsFrag, 0);

        //Fetch E
        vec3 tSize3 = vec3(textureSize(u_grid, 0));
        float E = dot(colRef.xyz, vec3(1.0)) * mul_E;
        E /= tSize3.z;

        //Fetch from the grid
        vec2 coord = (gl_FragCoord.xy * s_S) / tSize3.xy;
        vec4 sliced = texture(u_grid, vec3(coord.xy, E));

        vec3 color = sliced.w > 0.0 ? sliced.xyz / sliced.w : vec3(0.0);

        f_color = vec4(color.xyz, 1.0);
    }
                      );
}

PIC_INLINE void FilterGLSlicer::initShaders()
{
    technique.initStandard("400", vertex_source, fragment_source, "FilterGLSlicer");

    update(s_S, s_R);
}

PIC_INLINE void FilterGLSlicer::update(float s_S, float s_R)
{
    this->s_S = s_S;
    this->s_R = s_R;

    mul_E = s_R / 3.0f;

#ifdef PIC_DEBUG
    printf("Rate S: %f Rate R: %f Mul E: %f\n", s_S, s_R, mul_E);
#endif

    technique.bind();
    technique.setUniform1i("u_tex", 0);
    technique.setUniform1i("u_grid", 1);
    technique.setUniform1f("s_S", s_S);
    technique.setUniform1f("mul_E", mul_E);
    technique.unbind();
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_SLICER_HPP */
