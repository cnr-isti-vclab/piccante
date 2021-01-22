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

#ifndef PIC_UTIL_GL_PROGRAM_HPP
#define PIC_UTIL_GL_PROGRAM_HPP

#include <string>

#include "../../base.hpp"

namespace pic {

class ProgramGL
{
protected:
    GLint bCheckStatus;
    GLenum type;
    GLuint object;
    std::string source;
    std::string log;
    bool bType;

    /**
     * @brief checkShaderStatus
     * @return
     */
    bool checkShaderStatus()
    {
        log.clear();

        bCheckStatus = GL_FALSE;
        glGetShaderiv(object, GL_COMPILE_STATUS, &bCheckStatus);

        //get the log in case of failure
        if(bCheckStatus == GL_FALSE) {
            GLint max_length = -1;
            glGetShaderiv(object, GL_INFO_LOG_LENGTH, &max_length);

            std::vector<GLchar> error(max_length);
            glGetShaderInfoLog(object, max_length, &max_length, &error[0]);

            glDeleteShader(object);
            object = 0;

            log += "-----------------------------------------\n";
            log += "-- This shader was not compiled!\n";
            log += "-----------------------------------------\n";

            return false;
        } else {
            log += "-----------------------------------------\n";
            log += "-- This shader was compiled successfully!\n";
            log += "-----------------------------------------\n";

            return true;
        }
    }

    /**
     * @brief checkProgramStatus
     * @return
     */
    bool checkProgramStatus()
    {
        bCheckStatus = GL_FALSE;
        glGetProgramiv(object, GL_LINK_STATUS, &bCheckStatus);

        //get the log in case of failure
        if(bCheckStatus == GL_FALSE) {
            GLint max_length = -1;
            glGetProgramiv(object, GL_INFO_LOG_LENGTH, &max_length);

            std::vector<GLchar> error(max_length);
            glGetProgramInfoLog(object, max_length, &max_length, &error[0]);

            glDeleteProgram(object);
            object = 0;

            log += "----------------------------------------\n";
            log += "-- This program was not linked!\n";
            log += "----------------------------------------\n";

            return false;
        } else {
            log += "----------------------------------------\n";
            log += "-- This program was linked successfully!\n";
            log += "----------------------------------------\n";

            return true;
        }
    }

public:

    bool bCompiled;

    ProgramGL()
    {
        setNULL();
    }

    ProgramGL( std::string version,
               std::string extensions,
               std::string source,
               GLenum type)
    {
        setNULL();
        initShader(version, extensions, source, type);
    }

    ProgramGL(std::vector<ProgramGL*> &shaders)
    {
        initProgram(shaders);
    }

    ~ProgramGL()
    {
        if(object != 0) {
            if(bType) {
                glDeleteShader(object);
            } else {
                glDeleteProgram(object);
            }
            object = 0;
        }

        log.clear();
        source.clear();
    }

    /**
     * @brief SetNULL
     */
    void setNULL()
    {
        this->bCompiled = false;
        this->bType = true;
        this->type = 0;
        this->object = 0;
        this->log = "";
        this->source = "";
        this->bCheckStatus = GL_FALSE;
    }

    /**
     * @brief getObject
     * @return
     */
    GLuint getObject()
    {
        return object;
    }

    /**
     * @brief initProgram
     * @param shaders
     * @return
     */
    bool initProgram(std::vector<ProgramGL*> &shaders)
    {
        bType = false;
        object = glCreateProgram();

        for(uint i = 0; i < shaders.size(); i++)
        {
            GLuint tmp = shaders[i]->getObject();
            if(tmp != 0) {
                glAttachShader(object, tmp);
            }
        }

        glLinkProgram(object);

        bCompiled = checkProgramStatus();

        return bCompiled;
    }

    /**
     * @brief initShader
     * @param version
     * @param extensions
     * @param source
     * @param type
     * @return
     */
    bool initShader( std::string version_number,
                std::string extensions,
                std::string source,
                GLenum type)
    {
        this->type = type;
        this->bType = true;

        object = glCreateShader(type);

        this->source.clear();

        //create full source
        if(!version_number.empty()) {
            this->source += "#version ";
            this->source += version_number;
            this->source += "\n";
        }

        if(!extensions.empty()) {
            this->source += extensions;
            this->source += "\n";
        }

        if(!source.empty()) {
            this->source += source;
        }

        const GLchar *tmp = (const GLchar *) this->source.c_str();
        glShaderSource(object, 1, &tmp, NULL);
        glCompileShader(object);

        return checkShaderStatus();
    }

    /**
     * @brief printLog
     */
    void printLog()
    {
        printf("%s", log.c_str());
    }

};

} // end namespace pic

#endif /* PIC_UTIL_GL_PROGRAM_HPP */

