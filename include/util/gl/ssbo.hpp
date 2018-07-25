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

#ifndef PIC_UTIL_GL_SSBO_HPP
#define PIC_UTIL_GL_SSBO_HPP

#include <iostream>

namespace pic {

using namespace std;

#ifdef OPEN_GL_4_30
/**
 * @brief The Ssbo class: shader storage buffer object
 */
class Ssbo
{
protected:
    GLuint ssbo;
    total_size;

public:

    /**
     * @brief Ssbo
     */
    Ssbo()
    {
        total_size = 0;
        ssbo = 0;
    }

    ~Ssbo()
    {
        if(ssbo != 0) {
        }
    }

    /**
     * @brief init
     * @param size_buffer
     * @param size_of_type
     * @param data
     */
    void init(unsigned int size_buffer, unsigned int size_of_type, void *data)
    {
        total_size = size_buffer * size_of_type;
        glGenBuffers(1, &ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, posSSbo );
        glBufferData(GL_SHADER_STORAGE_BUFFER, total_size, data, GL_DYNAMIC_COPY );

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    /**
     * @brief update
     * @param data
     */
    void update(void *data)
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        GLvoid *p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
        memcpy(p, &data, total_size)
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }

    void bind(unsigned int index)
    {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, ssbo);
    }

    void unbind(unsigned int index)
    {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, 0);
    }

    /*
    void *mapBuffer()
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, posSSbo );
        GLint bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;
        return glMapBufferRange( GL_SHADER_STORAGE_BUFFER, 0, total_size, bufMask);
    }

    void unmapBuffer()
    {
        glUnmapBuffer( GL_SHADER_STORAGE_BUFFER );
    }
    */
};

#endif

}// end namespace pic


#endif /* PIC_UTIL_GL_SSBO_HPP */

