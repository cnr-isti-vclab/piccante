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

#ifndef PIC_UTIL_GRID_HPP
#define PIC_UTIL_GRID_HPP

#include <string>

#include "util/std_util.hpp"
#include "util/gl/technique.hpp"

class Grid {
public:

    int n;
    float length;
    GLuint vao, vbo, cbo, ibo;
    pic::TechniqueGL meshProgram;

    Grid(int n = 64, float length = 16.0f)
    {
        this->n = n;
        this->length = length;
    }

    void create()
    {
        float *vb = new float[n * 4 * 3];

        for (int i = 0; i < n; i++) {
            float x = (float(i) * length) / float(n - 1);

            int j = i * 6;
            vb[j    ] = x;
            vb[j + 1] = 0.0f;
            vb[j + 2] = 0.0f;

            vb[j + 3] = x;
            vb[j + 4] = 0.0f;
            vb[j + 5] = length;
        }

        int shift = n * 6;
        for (int i = 0; i < n; i++) {
            float z = (float(i) * length) / float(n - 1);

            int j = shift + i * 6;

            vb[j] = 0.0f;
            vb[j + 1] = 0.0f;
            vb[j + 2] = z;

            vb[j + 3] = length;
            vb[j + 4] = 0.0f;
            vb[j + 5] = z;
        }

        //
        //Vertex Buffers
        //
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        //Vertex buffer object
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * n * 12, vb, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);

        /*Create shader*/
        //Shader Program
        std::string vertex_source = MAKE_STRING
        (
            layout(location = 0) in vec4 a_position;
            uniform mat4 u_mvp;

            void main(void) {
                gl_Position = u_mvp * a_position;
            }
        );

        std::string fragment_source = MAKE_STRING
        (
            layout(location = 0) out vec4 f_color;

            void main(void) {
                f_color = vec4(0.8, 0.8, 0.8, 1.0);
            }
        );
        
        meshProgram.init("330", vertex_source, fragment_source, "MeshProgram");
        meshProgram.printLog("MeshProgram");

        meshProgram.bind();
        meshProgram.setAttributeIndex("a_position", 0);
        meshProgram.setOutputFragmentShaderIndex("f_color", 0);
        meshProgram.link();
        meshProgram.unbind();
    }

    void render(float *viewprojection)
    {
        meshProgram.bind();
        meshProgram.setUniform4x4("u_mvp", viewprojection, false);

        glBindVertexArray(vao);
        glDrawArrays(GL_LINES, 0, n * 4);
        glBindVertexArray(0);

        meshProgram.unbind();
    }

};



#endif /* PIC_UTIL_GRID_HPP */

