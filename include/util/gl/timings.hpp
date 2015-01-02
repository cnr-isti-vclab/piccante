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

#ifndef PIC_UTIL_GL_TIMINGS_HPP
#define PIC_UTIL_GL_TIMINGS_HPP

#include "gl.hpp"

namespace pic {

/**
 * @brief glBeginTimeQuery
 * @return
 */
inline GLuint glBeginTimeQuery()
{
    //Timing
    GLuint ret;
    glGenQueries(1, &ret);
    glFinish();
    #ifndef PIC_DISABLE_OPENGL_NON_CORE
    glBeginQuery(GL_TIME_ELAPSED_EXT, ret);
    #endif
    return ret;
}

/**
 * @brief glEndTimeQuery
 * @param ret
 * @return
 */
inline GLuint64EXT glEndTimeQuery(GLuint ret)
{
    GLuint64 timeVal = 0;
    //Timing
    #ifndef PIC_DISABLE_OPENGL_NON_CORE
    glEndQuery(GL_TIME_ELAPSED_EXT);
    glGetQueryObjectui64vEXT(ret, GL_QUERY_RESULT, &timeVal);
    #endif
    return timeVal;
}

} // end namespace pic

#endif /* PIC_UTIL_GL_TIMINGS_HPP */

