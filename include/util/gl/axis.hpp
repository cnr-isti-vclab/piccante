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

#ifndef PIC_UTIL_AXIS_HPP
#define PIC_UTIL_AXIS_HPP

#include <string>

#include "util/std_util.hpp"
#include "util/gl/technique.hpp"

class Axis {
public:

    GLuint vao, vbo, cbo, ibo;
    pic::TechniqueGL meshProgram;

    Axis()
    {

    }

    void create()
    {
        int n = 6 * 3;
        //
        //vertex array:
        //
        float *vb = new float[n];

        //first line X-axis 
        vb[0] = 0.0f;
        vb[1] = 0.0f;
        vb[2] = 0.0f;

        vb[3] = 1.0f;
        vb[4] = 0.0f;
        vb[5] = 0.0f;

        //second line Y-axis
        vb[6] = 0.0f;
        vb[7] = 0.0f;
        vb[8] = 0.0f;

        vb[9] = 0.0f;
        vb[10] = 1.0f;
        vb[11] = 0.0f;

        //third line Z-axis
        vb[12] = 0.0f;
        vb[13] = 0.0f;
        vb[14] = 0.0f;

        vb[15] = 0.0f;
        vb[16] = 0.0f;
        vb[17] = 1.0f;

        //color array
        unsigned char *cb = new unsigned char[n];

        //color of the X-axis: blue
        cb[0] = 255;
        cb[1] = 0;
        cb[2] = 0;

        cb[3] = 255;
        cb[4] = 0;
        cb[5] = 0;

        //color of the Y-axis: red
        cb[6] = 0;
        cb[7] = 255;
        cb[8] = 0;

        cb[9] = 0;
        cb[10] = 255;
        cb[11] = 0;

        //color of the Z-axis: green
        cb[12] = 0;
        cb[13] = 0;
        cb[14] = 255;

        cb[15] = 0;
        cb[16] = 0;
        cb[17] = 255;

        //
        //Vertex Buffers
        //
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        //Vertex buffer object
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * n, vb, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);

        //Color buffer object
        
        glGenBuffers(1, &cbo);
        glBindBuffer(GL_ARRAY_BUFFER, cbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned char) * n, cb, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);

        //
        //Index buffers
        //
        unsigned int *indeces = new unsigned int[6];
        
        //X-axis
        indeces[0] = 0;
        indeces[1] = 1;
        //Y-axis
        indeces[2] = 2;
        indeces[3] = 3;
        //Z-axis
        indeces[4] = 4;
        indeces[5] = 5;
        
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, &indeces[0], GL_STATIC_DRAW);

        /*Create shader*/
        //Shader Program
        std::string vertex_source = MAKE_STRING
        (
            layout(location = 0) in vec4 a_position;
            layout(location = 1) in vec3 a_color;
            uniform mat4 u_mvp;
            out vec4 v_color;

            void main(void) {
                gl_Position = u_mvp * a_position;//vec4(a_position.xyz, 1.0);
                v_color = vec4(a_color.xyz, 1.0);
            }
        );

        std::string fragment_source = MAKE_STRING
        (
            in vec4 v_color;
            layout(location = 0) out vec4 f_color;

            void main(void) {
                f_color = v_color;
            }
        );



        meshProgram.init("330", vertex_source, fragment_source, "MeshProgram");
        meshProgram.printLog("MeshProgram");

        meshProgram.bind();
        meshProgram.setAttributeIndex("a_position", 0);
        meshProgram.setAttributeIndex("a_color", 1);
        meshProgram.setOutputFragmentShaderIndex("f_color", 0);
        meshProgram.link();
        meshProgram.unbind();
    }

    void render(float *viewprojection, float linewidth = 4.0f)
    {
        glEnable(GL_LINE_SMOOTH);

        glLineWidth(linewidth);

        meshProgram.bind();
        meshProgram.setUniform4x4("u_mvp", viewprojection, false);

        glBindVertexArray(vao);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glDrawElements(GL_LINES,6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        meshProgram.unbind();

        glDisable(GL_LINE_SMOOTH);

    }

};



#endif /* PIC_UTIL_AXIS_HPP */

