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

#ifndef PIC_UTIL_GL_QUAD_HPP
#define PIC_UTIL_GL_QUAD_HPP

namespace pic {

#include <string>

class QuadGL
{

public:

    GLuint vao;		//vertex array object
    GLuint vbo[2];	//vertex buffer object

    QuadGL()
    {
        vao = 0;
        vbo[0] = 0;
        vbo[1] = 0;
    }

    ~QuadGL()
    {
        if(vao > 0) {
            glDeleteBuffers(1, &vao);
        }

        if(vbo[0] > 0) {
            glDeleteBuffers(1, &vbo[0]);
        }

        if(vbo[1] > 0) {
            glDeleteBuffers(1, &vbo[1]);
        }
    }

    /**Render: renders a quad*/
    void Render()
    {
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }

    /**CreatePosCoord: creates position coordiantes for a quad*/
    static float *CreatePosCoord()
    {
        float *data = new float[8];
        data[0] = -1.0f;
        data[1] =  1.0f;
        data[2] = -1.0f;
        data[3] = -1.0f;
        data[4] =  1.0f;
        data[5] =  1.0f;
        data[6] =  1.0f;
        data[7] = -1.0f;
        return data;
    }

    /*
        glTexCoord2f( 0.0f,  0.0f );

        glTexCoord2f( 0.0f,  1.0f );

        glTexCoord2f( 1.0f,  0.0f );

        glTexCoord2f( 1.0f,  1.0f );*/

    /**CreateTexCoord: creates texture coordiantes for a quad*/
    static float *CreateTexCoord()
    {
        float *data = new float[8];
        data[0] = 0.0f;
        data[1] = 1.0f;
        data[2] = 0.0f;
        data[3] = 0.0f;
        data[4] = 1.0f;
        data[5] = 1.0f;
        data[6] = 1.0f;
        data[7] = 0.0f;
        return data;
    }

    /**CreatePosTexQuad: creates a quad with position attribute*/
    static QuadGL *CreatePosQuad()
    {
        QuadGL *ret = new QuadGL();

        float *data_pos = CreatePosCoord();

        //Init VBO
        glGenBuffers(1, &ret->vbo[0]);
        glBindBuffer(GL_ARRAY_BUFFER, ret->vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), data_pos, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        //Init VAO
        glGenVertexArrays(1, &ret->vao);
        glBindVertexArray(ret->vao);
        glBindBuffer(GL_ARRAY_BUFFER, ret->vbo[0]);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
        glDisableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        delete[] data_pos;

        return ret;
    }

    /**CreatePosTexQuad: creates a quad with position and texture attributes*/
    static QuadGL *CreatePosTexQuad()
    {
        QuadGL *ret = new QuadGL();

        float *data_pos = CreatePosCoord();
        float *data_tex = CreateTexCoord();

        //Init VBO 0
        glGenBuffers(1, &ret->vbo[0]);
        glBindBuffer(GL_ARRAY_BUFFER, ret->vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), data_pos, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        //Init VBO 1
        glGenBuffers(1, &ret->vbo[1]);
        glBindBuffer(GL_ARRAY_BUFFER, ret->vbo[1]);
        glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), data_tex, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        //Init VAO
        glGenVertexArrays(1, &ret->vao);
        glBindVertexArray(ret->vao);

        glBindBuffer(GL_ARRAY_BUFFER, ret->vbo[0]);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, ret->vbo[1]);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
        glDisableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        delete[] data_pos;
        delete[] data_tex;

        return ret;
    }

    /**CreateSimpleVP: creates a basic vertex program*/
    static std::string CreateSimpleVP()
    {
        std::string vertex_program = GLW_STRINGFY
                                     (
                                         in vec3 a_position;

        void main(void) {
            gl_Position = vec4(a_position, 1.0);
        }
                                     );

        return vertex_program;
    }

    /**CreateSimpleVP_VAO: creates a basic vertex program*/
    static std::string CreateSimpleVP_VAO()
    {
        std::string vertex_program = GLW_STRINGFY
                                     (
                                         in vec2 a_position;

        void main(void) {
            gl_Position = vec4(a_position, 0.0, 1.0);
        }
                                     );

        return vertex_program;
    }

    static std::string CreateSimpleVPT_VAO()
    {
        std::string vertex_program = GLW_STRINGFY
                                     (
                                         in vec2 a_position;
                                         in vec2 a_tex_coord;
                                         out vec2 v_tex_coord;

        void main(void) {
            gl_Position = vec4(a_position, 0.0, 1.0);
            v_tex_coord = a_tex_coord;
        }
                                     );

        return vertex_program;
    }

    //Create a basic program
    static void CreateSimpleProgram(glw::program &ret)
    {
        std::string vertex_source = GLW_STRINGFY
                                    (
                                        in vec3 a_position;

        void main(void) {
            gl_Position = vec4(a_position, 1.0);
        }
                                    );

        std::string fragment_source = GLW_STRINGFY
                                      (
                                          uniform sampler2D u_tex;
                                          out     vec4      f_color;

        void main(void) {
            ivec2 tsize = textureSize2D(u_tex, 0);
            ivec2 coords = ivec2(gl_FragCoord.xy);
            coords.y = tsize.y - coords.y;
            f_color = vec4(texelFetch(u_tex, coords, 0).xyz, 1.0);
        }
                                      );

        std::string prefix;
        prefix += glw::version("150");
        prefix += glw::ext_require("GL_EXT_gpu_shader4");
        ret.setup(prefix, vertex_source, fragment_source);
        printf("[Simple Program log]\n%s\n", ret.log().c_str());

        glw::bind_program(ret);
        ret.attribute_source("a_position", 0);
        ret.fragment_target("f_color",    0);
        glw::bind_program(0);

        glw::bind_program(ret);
        ret.relink();
        ret.uniform("u_tex",      0);
        glw::bind_program(0);
    }

    /**Draw: draw using compability mode (deprecated!)*/
    static void Draw()
    {
        glDisable(GL_DEPTH_TEST);

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        //Rendering an aligned quad
        glBegin(GL_TRIANGLE_STRIP);

        glVertex2f(-1.0f,  1.0f);
        glVertex2f(-1.0f, -1.0f);
        glVertex2f(1.0f,  1.0f);
        glVertex2f(1.0f, -1.0f);

        glEnd();
    }

    /**Draw: draws using compability mode (deprecated!)*/
    static void Draw(GLuint tex)
    {
        glEnable(GL_TEXTURE_2D);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        //Rendering an aligned quad
        glBegin(GL_TRIANGLE_STRIP);

        glTexCoord2f(0.0f,  0.0f);
        glVertex2f(-1.0f,  1.0f);

        glTexCoord2f(0.0f,  1.0f);
        glVertex2f(-1.0f, -1.0f);

        glTexCoord2f(1.0f,  0.0f);
        glVertex2f(1.0f,  1.0f);

        glTexCoord2f(1.0f,  1.0f);
        glVertex2f(1.0f, -1.0f);

        glEnd();

        glDisable(GL_TEXTURE_2D);
    }

    /**Draw: draws using compability mode (deprecated!)*/
    static void Draw(GLuint tex, float *color)
    {
        glEnable(GL_TEXTURE_2D);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);

        if(color == NULL) {
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        } else {
            glColor3fv(color);
        }

        //Rendering an aligned quad
        glBegin(GL_TRIANGLE_STRIP);

        glTexCoord2f(0.0f,  0.0f);
        glVertex2f(-1.0f,  1.0f);

        glTexCoord2f(0.0f,  1.0f);
        glVertex2f(-1.0f, -1.0f);

        glTexCoord2f(1.0f,  0.0f);
        glVertex2f(1.0f,  1.0f);

        glTexCoord2f(1.0f,  1.0f);
        glVertex2f(1.0f, -1.0f);

        glEnd();

        glDisable(GL_TEXTURE_2D);
    }

    static void Draw(GLuint texture, int width, int height, glw::program &pg)
    {
        glFrontFace(GL_CW);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glDisable(GL_DEPTH_TEST);
        glViewport(0, 0, (GLsizei)width, (GLsizei)height);

        glw::bind_program(pg);

        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        //Rendering an aligned quad
        glBegin(GL_TRIANGLE_STRIP);
        glVertex2f(1.0f, -1.0f);

        glVertex2f(-1.0f, -1.0f);

        glVertex2f(1.0f,  1.0f);

        glVertex2f(-1.0f,  1.0f);

        glEnd();

        glDisable(GL_TEXTURE_2D);
        glw::bind_program(0);
        glEnable(GL_DEPTH_TEST);
    }
};

} // end namespace pic

#endif /* PIC_UTIL_GL_QUAD_HPP */

