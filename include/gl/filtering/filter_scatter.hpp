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

#include "../../base.hpp"
#include "../../util/std_util.hpp"
#include "../../gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLScatter class implement
 * the bilateral grid approximation of the bilateral
 * filter.
 */
class FilterGLScatter: public FilterGL
{
protected:

    GLfloat *vertex_array;
    int nVertex_array;
    GLuint vbo, vao;

    /**
     * @brief generateVertexArray
     * @param width
     * @param height
     */
    void generateVertexArray(int width, int height);

    /**
     * @brief initShaders
     */
    void initShaders();

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
     * @brief releaseAux
     */
    void releaseAux()
    {
        vertex_array = delete_vec_s(vertex_array);

        if(vbo != 0) {
            glDeleteBuffers(1, &vbo);
            vbo = 0;
        }

        if(vao != 0) {
            glDeleteVertexArrays(1, &vao);
            vao = 0;
        }
    }

    /**
     * @brief update
     * @param s_S
     * @param s_R
     */
    void update(float s_S, float s_R);

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageGL *Process(ImageGLVec imgIn, ImageGL *imgOut);
};

PIC_INLINE FilterGLScatter::FilterGLScatter(float s_S, float s_R, int width, int height)
{
    this->s_S = s_S;
    this->s_R = s_R;

    vertex_array = NULL;

    generateVertexArray(width, height);

    FragmentShader();
    initShaders();
}

PIC_INLINE FilterGLScatter::~FilterGLScatter()
{
    release();
}

PIC_INLINE void FilterGLScatter::generateVertexArray(int width, int height)
{
    vertex_array = delete_vec_s(vertex_array);

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

PIC_INLINE void FilterGLScatter::FragmentShader()
{
    vertex_source = MAKE_STRING(

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

    geometry_source = MAKE_STRING(

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

    fragment_source = MAKE_STRING(
                          flat in	vec4	g2f_color;
                          layout(location = 0) out vec4    f_color;

    void main(void) {
        f_color = g2f_color;
    }
                      );
}

PIC_INLINE void FilterGLScatter::initShaders()
{
    technique.initStandard("410", vertex_source, fragment_source, geometry_source, "FilterGLScatter");

    update(s_S, s_R);
}

PIC_INLINE void FilterGLScatter::update(float s_S, float s_R)
{
    this->s_S = s_S;
    this->s_R = s_R;

    mul_E = s_R / 3.0f;

    #ifdef PIC_DEBUG
        printf("Rate S: %f Rate R: %f Mul E: %f\n", s_S, s_R, mul_E);
    #endif

    technique.bind();
    technique.setUniform1i("u_tex", 0);
    technique.setUniform1f("s_S", s_S);
    technique.setUniform1f("mul_E", mul_E);
    technique.unbind();
}

PIC_INLINE ImageGL *FilterGLScatter::Process(ImageGLVec imgIn, ImageGL *imgOut)
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
    technique.bind();

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
    technique.unbind();

    //Textures
    glActiveTexture(GL_TEXTURE0);
    imgIn[0]->unBindTexture();

    return imgOut;
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_SCATTER_HPP */

