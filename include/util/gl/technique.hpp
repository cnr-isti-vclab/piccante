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

#ifndef PIC_UTIL_GL_TECHNIQUE_HPP
#define PIC_UTIL_GL_TECHNIQUE_HPP

#include <string>

#include "../../util/std_util.hpp"
#include "../../util/gl/program.hpp"

namespace pic {

/**
 * @brief The TechniqueGL class
 */
class TechniqueGL
{
protected:
    std::vector<ProgramGL*> shaders;
    ProgramGL main;

    /**
     * @brief getLocation
     * @param name
     * @return
     */
    GLuint getLocation(const char *name)
    {
        return glGetUniformLocation(main.getObject(), name);
    }

public:

    /**
     * @brief TechniqueGL
     */
    TechniqueGL()
    {

    }

    ~TechniqueGL()
    {
        stdVectorClear<ProgramGL>(shaders);
    }

    bool isValid()
    {
        return main.bCompiled;
    }

    bool init( std::string version_number,
               std::string vertex_shader_source,
               std::string fragment_shader_source)
    {
        ProgramGL *vss = new ProgramGL(version_number, "", vertex_shader_source, GL_VERTEX_SHADER);
        ProgramGL *fss = new ProgramGL(version_number, "", fragment_shader_source, GL_FRAGMENT_SHADER);
        shaders.push_back(vss);
        shaders.push_back(fss);

        return main.initProgram(shaders);
    }

    bool init( std::string version_number,
               std::string vertex_shader_source,
               std::string fragment_shader_source,
               std::string geomety_shader_source)
    {
        ProgramGL *vss = new ProgramGL(version_number, "", vertex_shader_source, GL_VERTEX_SHADER);
        ProgramGL *gss = new ProgramGL(version_number, "", geomety_shader_source, GL_GEOMETRY_SHADER);
        ProgramGL *fss = new ProgramGL(version_number, "", fragment_shader_source, GL_FRAGMENT_SHADER);

        shaders.push_back(vss);
        shaders.push_back(gss);
        shaders.push_back(fss);

        return main.initProgram(shaders);
    }

    bool initCompute(std::string version_number,
                     std::string compute_shader_source)
    {
        #ifdef OPEN_GL_4_30
            ProgramGL *css = new ProgramGL(version_number, "", compute_shader_source, GL_COMPUTE_SHADER);
            shaders.push_back(css);
        #endif

        return main.initProgram(shaders);
    }

    /**
     * @brief initStandard
     * @param version_number
     * @param vertex_shader_source
     * @param fragment_shader_source
     * @param name
     * @return
     */
    bool initStandard( std::string version_number,
                    std::string vertex_shader_source,
                    std::string fragment_shader_source,
                    std::string name)
    {
        bool bCheck = this->init(version_number,
                                vertex_shader_source,
                                fragment_shader_source);

    #ifdef PIC_DEBUG
        this->printLog(name);
    #endif

        if (bCheck) {
            this->bind();
            this->setAttributeIndex("a_position", 0);
            this->setOutputFragmentShaderIndex("f_color", 0);
            this->link();
            this->unbind();
        }

        return bCheck;
    }

    /**
     * @brief initStandard
     * @param version_number
     * @param vertex_shader_source
     * @param fragment_shader_source
     * @param geometry_shader_source
     * @param name
     * @return
     */
    bool initStandard( std::string version_number,
                       std::string vertex_shader_source,
                       std::string fragment_shader_source,
                       std::string geometry_shader_source,
                       std::string name)
    {
        bool bCheck = this->init(version_number,
                   vertex_shader_source,
                   fragment_shader_source,
                   geometry_shader_source);

    #ifdef PIC_DEBUG
        this->printLog(name);
    #endif
        if (bCheck) {
            this->bind();
            this->setAttributeIndex("a_position", 0);
            this->setOutputFragmentShaderIndex("f_color", 0);
            this->link();
            this->unbind();
        }

        return bCheck;
    }

    /**
     * @brief printLog
     * @param name
     */
    void printLog(std::string name)
    {
        printf("\nLog for: %s\n", name.c_str());
        for(uint i = 0; i < shaders.size(); i++) {
            shaders[i]->printLog();
        }

        main.printLog();
    }

    /**
     * @brief bind
     */
    void bind()
    {
        glUseProgram(main.getObject());
    }

    /**
     * @brief unbind
     */
    void unbind()
    {
        glUseProgram(0);
    }

    /**
     * @brief link
     */
    void link()
    {
        glLinkProgram(main.getObject());
    }

    /**
     * @brief setOutputFragmentShaderIndex
     * @param fragment_output_color_name
     * @param index
     */
    void setOutputFragmentShaderIndex(const char *fragment_output_color_name, unsigned int index)
    {
        glBindFragDataLocation(main.getObject(), GLuint(index), fragment_output_color_name);
    }

    /**
     * @brief setAttributeIndex
     * @param attribute_name
     * @param index
     */
    void setAttributeIndex(const char *attribute_name, unsigned int index)
    {
        glBindAttribLocation(main.getObject(), GLuint(index), attribute_name);
    }


    /**
     * @brief SetUniform
     * @param name_uniform
     * @param value0
     */
    void setUniform1i(const char *name_uniform, int value0)
    {
        glUniform1i(getLocation(name_uniform),
                    GLint(value0));
    }

    /**
     * @brief SetUniform1f
     * @param name_uniform
     * @param value0
     */
    void setUniform1f(const char *name_uniform, float value0)
    {
        glUniform1f(getLocation(name_uniform),
                    GLfloat(value0));
    }

    /**
     * @brief setUniform
     * @param name_uniform
     * @param value0
     * @param value1
     */
    void setUniform2f(const char *name_uniform, float value0, float value1)
    {
        glUniform2f(getLocation(name_uniform),
                    GLfloat(value0),
                    GLfloat(value1));
    }

    /**
     * @brief setUniform
     * @param name_uniform
     * @param value0
     * @param value1
     * @param value2
     */
    void setUniform3f(const char *name_uniform, float value0, float value1, float value2)
    {
        glUniform3f(getLocation(name_uniform),
                    GLfloat(value0),
                    GLfloat(value1),
                    GLfloat(value2));
    }

    /**
     * @brief setUniform4f
     * @param name_uniform
     * @param value0
     * @param value1
     * @param value2
     * @param value3
     */
    void setUniform4f(const char *name_uniform, float value0, float value1, float value2, float value3)
    {
        glUniform4f(getLocation(name_uniform),
                    GLfloat(value0),
                    GLfloat(value1),
                    GLfloat(value2),
                    GLfloat(value3));
    }

    /**
     * @brief setUniform3x3
     * @param name_uniform
     * @param matrix
     * @param bTranspose
     */
    void setUniform3x3(const char *name_uniform, const float *matrix, bool bTranspose)
    {
        glUniformMatrix3fv(getLocation(name_uniform),
                           GLsizei(1),
                           bTranspose ? GL_TRUE : GL_FALSE,
                           (const GLfloat*)(matrix));
    }

    /**
     * @brief setUniform4x4
     * @param name_uniform
     * @param matrix
     * @param bTranspose
     */
    void setUniform4x4(const char *name_uniform, const float *matrix, bool bTranspose)
    {
        glUniformMatrix4fv(getLocation(name_uniform),
                           GLsizei(1),
                           bTranspose ? GL_TRUE : GL_FALSE,
                           (const GLfloat*)(matrix));
    }

    /**
     * @brief setUniform3
     * @param name_uniform
     * @param value
     */
    void setUniform3fv(const char *name_uniform, const float *value)
    {
        glUniform3fv(getLocation(name_uniform),
                     GLsizei(1),
                     (const GLfloat *)value);
    }

    /**
     * @brief setUniform4
     * @param name_uniform
     * @param value
     */
    void setUniform4fv(const char *name_uniform, const float *value)
    {
        glUniform4fv(getLocation(name_uniform),
                     GLsizei(1),
                     (const GLfloat *)value);
    }

    #ifdef OPEN_GL_4_30
    void setSSBOIndex(const char *ssbo_name, unsigned int index)
    {
        GLunit block_index = 0;
        block_index = glGetProgramResourceIndex(main, GL_SHADER_STORAGE_BLOCK, ssbo_name);
        glShaderStorageBlockBinding(program, block_index, index);
    }
    #endif
};

} // end namespace pic

#endif /* PIC_UTIL_GL_TECHNIQUE_HPP */

