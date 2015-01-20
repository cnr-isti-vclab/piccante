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

#ifndef PIC_GL_FILTERING_FILTER_ANISOTROPIC_DIFFUSION_HPP
#define PIC_GL_FILTERING_FILTER_ANISOTROPIC_DIFFUSION_HPP

#include "gl/filtering/filter.hpp"
#include "gl/filtering/filter_iterative.hpp"

namespace pic {

class FilterGLAnisotropicDiffusion: public FilterGL
{
protected:
    void InitShaders();
    void FragmentShader();

    float				delta_t, k;
    unsigned int		iterations;
    FilterGLIterative	*flt;

public:
    //Basic constructors
    FilterGLAnisotropicDiffusion(float k, unsigned int iterations);
    FilterGLAnisotropicDiffusion(float sigma_r, float sigma_s);

    //Updating k
    void Update(float k);

    //Processing
    ImageGL *Process(ImageGLVec imgIn, ImageGL *imgOut);

    //Anisotropic Diffusion
    ImageGL *AnisotropicDiffusion(ImageGLVec imgIn, ImageGL *imgOut)
    {
        if(flt == NULL) {
            flt = new FilterGLIterative(this, iterations);
        }

        return flt->Process(imgIn, imgOut);
    }
};

//Basic constructor
FilterGLAnisotropicDiffusion::FilterGLAnisotropicDiffusion(float k,
        unsigned int iterations): FilterGL()
{
    if(k <= 0.0f) {
        k = 0.11f;
    }

    if(iterations < 1) {
        iterations = 1;
    }

    flt = NULL;

    this->k = k;
    this->delta_t = 1.0f / 7.0f;
    this->iterations = iterations;

    //protected values are assigned/computed
    FragmentShader();
    InitShaders();
}

FilterGLAnisotropicDiffusion::FilterGLAnisotropicDiffusion(float sigma_r,
        float sigma_s): FilterGL()
{
    if(sigma_r <= 0.0f) {
        sigma_r = 0.11f;
    }

    flt = NULL;

    this->k = sigma_r;
    this->delta_t = 1.0f / 7.0f;

    iterations = int(ceilf(5.0f * sigma_s));

    //protected values are assigned/computed
    FragmentShader();
    InitShaders();
}

void FilterGLAnisotropicDiffusion::FragmentShader()
{
    fragment_source = GLW_STRINGFY
                      (
                          uniform sampler2D u_tex; \n
                          uniform float	  k2; \n
                          uniform float	  delta_t; \n
                          out     vec4      f_color; \n

    void main(void) {
        \n
        ivec2 coords = ivec2(gl_FragCoord.xy);
        \n
        vec3 cB = texelFetch(u_tex, coords           , 0).xyz;
        \n
        vec3 c0 = texelFetch(u_tex, coords + ivec2(1, 0), 0).xyz;
        \n
        vec3 c1 = texelFetch(u_tex, coords - ivec2(1, 0), 0).xyz;
        \n
        vec3 c2 = texelFetch(u_tex, coords + ivec2(0, 1), 0).xyz;
        \n
        vec3 c3 = texelFetch(u_tex, coords - ivec2(0, 1), 0).xyz;
        \n
        vec3 gN = c2 - cB;
        \n
        vec3 gS = c3 - cB;
        \n
        vec3 gW = c1 - cB;
        \n
        vec3 gE = c0 - cB;
        \n
        vec4 c = vec4(dot(gN, gN), dot(gS, gS), dot(gW, gW), dot(gE, gE));
        \n
        c = exp(-c / vec4(k2));
        \n
        f_color = vec4(cB + delta_t *(c.x * gN + c.y * gS + c.z * gW + c.w * gE), 1.0);
        \n
    }
                      );
}

void FilterGLAnisotropicDiffusion::InitShaders()
{

    FragmentShader();

    std::string prefix;

    filteringProgram.setup(glw::version("330"), vertex_source, fragment_source);

#ifdef PIC_DEBUG
    printf("[filteringProgram log]\n%s\n", filteringProgram.log().c_str());
#endif

    glw::bind_program(filteringProgram);
    filteringProgram.attribute_source("a_position", 0);
    filteringProgram.fragment_target("f_color",    0);
    filteringProgram.relink();
    Update(k);
}

void FilterGLAnisotropicDiffusion::Update(float k)
{
    if(k > 0.0f) {
        this->k = k;
    }

    float k2 = this->k * this->k;

    glw::bind_program(filteringProgram);
    filteringProgram.uniform("u_tex",      0);
    filteringProgram.uniform("k2",		k2);
    filteringProgram.uniform("delta_t",	delta_t);
    glw::bind_program(0);
}

//Processing
ImageGL *FilterGLAnisotropicDiffusion::Process(ImageGLVec imgIn,
        ImageGL *imgOut)
{
    if(imgIn[0] == NULL) {
        return imgOut;
    }

    int w = imgIn[0]->width;
    int h = imgIn[0]->height;

    if(imgOut == NULL) {
        imgOut = new ImageGL(imgIn[0]->frames, w, h, imgIn[0]->channels, IMG_GPU, GL_TEXTURE_2D);
    }

    if(fbo == NULL) {
        fbo = new Fbo();
    }

    fbo->create(w, h, imgIn[0]->frames, false, imgOut->getTexture());

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
    glBindTexture(GL_TEXTURE_2D, 0);

    return imgOut;
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_ANISOTROPIC_DIFFUSION_HPP */

