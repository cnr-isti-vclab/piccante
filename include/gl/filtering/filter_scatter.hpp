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

#ifndef PIC_GL_FILTERING_FILTER_SCATTER_HPP
#define PIC_GL_FILTERING_FILTER_SCATTER_HPP

#include "gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLScatter class implement
 * the bilateral grid approximation of the bilateral
 * filter.
 */
class FilterGLScatter: public FilterGL
{
protected:

    GLfloat		*vertex_array;
    int			nVertex_array;
    GLuint		vbo, vao;

    /**
     * @brief GenerateVA
     * @param width
     * @param height
     */
    void GenerateVA(int width, int height);

    /**
     * @brief InitShaders
     */
    void InitShaders();

    /**
     * @brief FragmentShader
     */
    void FragmentShader();

    float s_S, s_R, mul_E;

public:

    /**
     * @brief FilterGLScatter
     * @param s_S
     * @param s_R
     * @param width
     * @param height
     */
    FilterGLScatter(float s_S, float s_R, int width, int height);

    ~FilterGLScatter();

    /**
     * @brief Update
     * @param s_S
     * @param s_R
     */
    void Update(float s_S, float s_R);

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageGL *Process(ImageGLVec imgIn, ImageGL *imgOut);
};

FilterGLScatter::FilterGLScatter(float s_S, float s_R, int width, int height)
{
    this->s_S = s_S;
    this->s_R = s_R;

    GenerateVA(width, height);

    FragmentShader();
    InitShaders();
}

FilterGLScatter::~FilterGLScatter()
{
    if(vertex_array != NULL) {
        delete[] vertex_array;
        vertex_array = NULL;
    }

    if(vbo != 0) {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }

    if(vao != 0) {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }
}

void FilterGLScatter::GenerateVA(int width, int height)
{
    vertex_array = new GLfloat[2 * width * height];
    nVertex_array = width * height;

    int index = 0;

    for(int i = 0; i < height; i++) {
        float i_f = float(i);

        for(int j = 0; j < width; j++) {
            vertex_array[index++] = float(j);
            vertex_array[index++] = i_f;
        }
    }

    //Vertex Buffer Object
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 2 * nVertex_array * sizeof(GLfloat), vertex_array,
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //Vertex Array Object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void FilterGLScatter::FragmentShader()
{
    vertex_source = GLW_STRINGFY(

                        uniform sampler2D	u_tex;
                        uniform float		s_S;
                        uniform float		mul_E;

                        layout(location = 0) in vec2 a_position;

                        flat out vec4 v2g_color;
                        flat out int  v2g_layer;

    void main(void) {
        //Texture Fetch
        vec4 data = texelFetch(u_tex, ivec2(a_position), 0);

        //Output coordinate
        vec2 coord = vec2(a_position) / vec2(textureSize(u_tex, 0) - ivec2(1));
        coord = coord * 2.0 - vec2(1.0);

        v2g_color  = vec4(data.xyz, 1.0);
        v2g_layer  = int(floor(dot(data.xyz, vec3(1.0)) * mul_E));

        gl_Position = vec4(coord, 0.0, 1.0);
    }
                    );

    geometry_source = GLW_STRINGFY(

                          layout(points) in;
                          layout(points, max_vertices = 1) out;

                          flat in vec4  v2g_color[1];
                          flat in int   v2g_layer[1];
                          flat out vec4 g2f_color;

    void main(void) {
        g2f_color   = v2g_color[0];
        gl_Layer    = v2g_layer[0];

        gl_PointSize = 1.0;
        gl_Position = gl_in[0].gl_Position;
        EmitVertex();

        EndPrimitive();
    }
                      );

    fragment_source = GLW_STRINGFY(
                          flat in	vec4	g2f_color;
                          layout(location = 0) out vec4    f_color;

    void main(void) {
        f_color = g2f_color;
    }
                      );
}

void FilterGLScatter::InitShaders()
{
    filteringProgram.setup(glw::version("330"), vertex_source, geometry_source, fragment_source,
                           GL_POINTS, GL_POINTS, 1);

#ifdef PIC_DEBUG
    printf("[FilterGLScatter shader log]\n%s\n", filteringProgram.log().c_str());
#endif

    glw::bind_program(filteringProgram);

    filteringProgram.attribute_source("a_position", 0);
    filteringProgram.fragment_target("f_color",    0);
    filteringProgram.relink();

    glw::bind_program(0);

    Update(s_S, s_R);
}

void FilterGLScatter::Update(float s_S, float s_R)
{
    this->s_S = s_S;
    this->s_R = s_R;

    mul_E = s_R / 3.0f;

    #ifdef PIC_DEBUG
        printf("Rate S: %f Rate R: %f Mul E: %f\n", s_S, s_R, mul_E);
    #endif

    glw::bind_program(filteringProgram);
    filteringProgram.uniform("u_tex", 0);
    filteringProgram.uniform("s_S", s_S);
    filteringProgram.uniform("mul_E", mul_E);
    glw::bind_program(0);
}

ImageGL *FilterGLScatter::Process(ImageGLVec imgIn, ImageGL *imgOut)
{
    if(imgIn.size() < 1 && imgIn[0] == NULL) {
        return imgOut;
    }

    int width, height, range;
    width =  int(ceilf(float(imgIn[0]->width)  * s_S));
    height = int(ceilf(float(imgIn[0]->height) * s_S));
    range =  int(ceilf(s_R));

    if(imgOut == NULL) {
        imgOut = new ImageGL(range + 1, width + 1, height + 1,
                                imgIn[0]->channels + 1, IMG_GPU, GL_TEXTURE_3D);
    }

    if(fbo == NULL) {
        fbo = new Fbo();
        fbo->create(width + 1, height + 1, range + 1, false, imgOut->getTexture());
    }

    //Rendering
    fbo->bind();

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                            imgOut->getTexture(), 0);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(0, 0, (GLsizei)width, (GLsizei)height);

    //Shaders
    glw::bind_program(filteringProgram);

    //Textures
    glActiveTexture(GL_TEXTURE0);
    imgIn[0]->bindTexture();

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, nVertex_array);
    glBindVertexArray(0);

    glDisable(GL_BLEND);

    //Fbo
    fbo->unbind();

    //Shaders
    glw::bind_program(0);

    //Textures
    glActiveTexture(GL_TEXTURE0);
    imgIn[0]->unBindTexture();

    return imgOut;
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_SCATTER_HPP */

