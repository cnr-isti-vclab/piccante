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

#ifndef PIC_UTIL_GL_QUAD_HPP
#define PIC_UTIL_GL_QUAD_HPP

namespace pic {

#include <string>

#include "../../util/gl/technique.hpp"

/**
 * @brief The QuadGL class
 */
class QuadGL
{
protected:
    GLuint vao;		//vertex array object
    GLuint vbo[2];	//vertex buffer object

public:

    QuadGL()
    {
        vao = 0;
        vbo[0] = 0;
        vbo[1] = 0;
    }

    QuadGL(bool bTextureCoordinates, float halfSizeX = 1.0f, float halfSizeY = 1.0f)
    {
        vao = 0;
        vbo[0] = 0;
        vbo[1] = 0;

        init(bTextureCoordinates, halfSizeX, halfSizeY);
    }

    ~QuadGL()
    {
        if(vao > 0) {
            glDeleteBuffers(1, &vao);
            vao = 0;
        }

        if(vbo[0] > 0) {
            glDeleteBuffers(1, &vbo[0]);

            vbo[0] = 0;
        }

        if(vbo[1] > 0) {
            glDeleteBuffers(1, &vbo[1]);

            vbo[1] = 0;
        }
    }

    /**
     * @brief init initializates the QuadGL by allocating memory on the GPU.
     * @param bTextCoordinates
     */
    void init(bool bTexCoordinates, float halfSizeX = 1.0f, float halfSizeY = 1.0f)
    {
        float *data_pos = createPosCoord(halfSizeX, halfSizeY);

        if(bTexCoordinates) {
            float *data_tex = createTexCoord();

            //init VBO 0
            glGenBuffers(1, &vbo[0]);
            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
            glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), data_pos, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            //init VBO 1
            glGenBuffers(1, &vbo[1]);
            glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
            glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), data_tex, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            //init VAO
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);

            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(0);

            glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(1);

            glBindVertexArray(0);
            glDisableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            delete[] data_tex;
        } else {
            //init VBO
            glGenBuffers(1, &vbo[0]);
            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
            glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), data_pos, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            //init VAO
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

            glEnableVertexAttribArray(0);
            glBindVertexArray(0);
            glDisableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        delete[] data_pos;
    }

    /**
     * @brief Render draws a quad on screen.
     */
    void Render()
    {
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }

    /**
     * @brief Render
     * @param technque
     * @param texture
     */
    void Render(TechniqueGL &technique, GLuint texture)
    {
        technique.bind();

        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        Render();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        technique.unbind();
    }

    /**
     * @brief createPosCoord allocates memory for a position buffer.
     * @return
     */
    static float *createPosCoord(float halfSizeX = 1.0f, float halfSizeY = 1.0f)
    {
        float *data = new float[8];

        data[0] = -halfSizeX;
        data[1] =  halfSizeY;

        data[2] = -halfSizeX;
        data[3] = -halfSizeY;

        data[4] =  halfSizeX;
        data[5] =  halfSizeY;

        data[6] =  halfSizeX;
        data[7] = -halfSizeY;
        return data;
    }

    /**
     * @brief createTexCoord allocates memory for a texture coordinates buffer.
     * @return
     */
    static float *createTexCoord()
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

    /**
     * @brief getVertexProgramV3 creates a simple vertex program.
     * @return
     */
    static std::string getVertexProgramV3()
    {
        std::string vertex_program = MAKE_STRING
                                     (
                                         in vec3 a_position;

        void main(void) {
            gl_Position = vec4(a_position, 1.0);
        }
                                     );

        return vertex_program;
    }

    /**
     * @brief getVertexProgramV2 creates a simple vertex program.
     * @return
     */
    static std::string getVertexProgramV2()
    {
        std::string vertex_program = MAKE_STRING
                                     (
                                         in vec2 a_position;

        void main(void) {
            gl_Position = vec4(a_position, 0.0, 1.0);
        }
                                     );

        return vertex_program;
    }

    /**
     * @brief getVertexProgramWithTexCoordinates creates a simple vertex program
     * with texture coordinates as input.
     * @return
     */
    static std::string getVertexProgramWithTexCoordinates()
    {
        std::string vertex_program = MAKE_STRING
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

    /**
     * @brief getFragmentProgram
     * @return
     */
    static std::string getFragmentProgram()
    {

        std::string fragment_program = MAKE_STRING
                                      (
                                          uniform sampler2D u_tex;
                                          out     vec4      f_color;

        void main(void) {
            ivec2 coords = ivec2(gl_FragCoord.xy);
            f_color = vec4(texelFetch(u_tex, coords, 0).xyz, 1.0);
        }
                                      );

        return fragment_program;
    }

    /**
     * @brief getFragmentProgramForView
     * @return
     */
    static std::string getFragmentProgramForView()
    {

        std::string fragment_program = MAKE_STRING
                                      (
                                          uniform sampler2D u_tex;
                                          out     vec4      f_color;

        void main(void) {
            ivec2 coords = ivec2(gl_FragCoord.xy);
            ivec2 texSize = textureSize(u_tex, 0);
            coords.y = texSize.y - coords.y;
            f_color = vec4(texelFetch(u_tex, coords, 0).xyz, 1.0);
        }
                                      );

        return fragment_program;
    }

    /**
     * @brief getProgram creates a simple program.
     * @param ret
     * @param vp_src
     * @param fp_src
     */
    static void getTechnique(TechniqueGL &technique, std::string vp_src = "", std::string fp_src = "", bool bTextureCoordinates = false)
    {
        if(vp_src.empty() || fp_src.empty()) {
            technique.init("330", getVertexProgramV3(), getFragmentProgram());
        } else {
            technique.init("330", vp_src, fp_src);
        }

        #ifdef PIC_DEBUG
            technique.printLog("QuadGL");
        #endif

        technique.bind();
        technique.setAttributeIndex("a_position", 0);

        if(bTextureCoordinates) {
            technique.setAttributeIndex("a_tex_coord", 1);
        }

        technique.setOutputFragmentShaderIndex("f_color", 0);
        technique.link();
        technique.unbind();

        technique.bind();
        technique.setUniform1i("u_tex", 0);
        technique.unbind();
    }

    #ifdef PIC_DEPRECATED
    /**
     * @brief Draw: draw using compability mode (deprecated!)
     */
    static void Draw()
    {
        glDisable(GL_DEPTH_TEST);

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        //Rendering an aligned quad
        glBegin(GL_TRIANGLE_STRIP);

        glVertex2f( -1.0f,  1.0f);
        glVertex2f( -1.0f, -1.0f);
        glVertex2f(  1.0f,  1.0f);
        glVertex2f(  1.0f, -1.0f);

        glEnd();
    }

    /**
     * @brief Draw draws using compability mode (deprecated!).
     * @param tex
     */
    static void Draw(GLuint tex)
    {
        glEnable(GL_TEXTURE_2D);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        //Rendering an aligned quad
        glBegin(GL_TRIANGLE_STRIP);

        glTexCoord2f( 0.0f,  0.0f);
        glVertex2f(  -1.0f,  1.0f);

        glTexCoord2f( 0.0f,  1.0f);
        glVertex2f(  -1.0f, -1.0f);

        glTexCoord2f( 1.0f,  0.0f);
        glVertex2f(   1.0f,  1.0f);

        glTexCoord2f( 1.0f,  1.0f);
        glVertex2f(   1.0f, -1.0f);

        glEnd();

        glDisable(GL_TEXTURE_2D);
    }

    /**
     * @brief Draw draws using compability mode (deprecated!).
     * @param tex
     * @param color
     */
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
        glVertex2f( -1.0f,  1.0f);

        glTexCoord2f(0.0f,  1.0f);
        glVertex2f( -1.0f, -1.0f);

        glTexCoord2f(1.0f,  0.0f);
        glVertex2f(  1.0f,  1.0f);

        glTexCoord2f(1.0f,  1.0f);
        glVertex2f(  1.0f, -1.0f);

        glEnd();

        glDisable(GL_TEXTURE_2D);
    }

    /**
     * @brief Draw
     * @param texture
     * @param width
     * @param height
     * @param pg
     */
    static void Draw(GLuint texture, int width, int height, TechniqueGL &technique)
    {
        glFrontFace(GL_CW);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glDisable(GL_DEPTH_TEST);
        glViewport(0, 0, (GLsizei)width, (GLsizei)height);

        technique.bind();

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

        technique.unbind();
        glEnable(GL_DEPTH_TEST);        
    }
    #endif // end PIC_DEPRECATED
};

} // end namespace pic

#endif /* PIC_UTIL_GL_QUAD_HPP */

