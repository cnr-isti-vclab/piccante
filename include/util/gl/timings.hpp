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

