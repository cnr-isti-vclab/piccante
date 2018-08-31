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

#include "../../gl.hpp"

namespace pic {

/**
 * @brief glBeginTimeQuery
 * @return
 */
inline GLuint glBeginTimeQuery()
{
    GLuint ret = 0;
#ifdef PIC_GL_TIMING
    glGenQueries(1, &ret);
    glFinish();
    glBeginQuery(GL_TIME_ELAPSED, ret);
#endif

    return ret;
}

/**
 * @brief glEndTimeQuery
 * @param ret
 * @return
 */
inline GLuint64 glEndTimeQuery(GLuint64 ret)
{
    GLuint64 timeVal = 0;

#ifdef PIC_GL_TIMING
    glEndQuery(GL_TIME_ELAPSED);
    glGetQueryObjectui64v(ret, GL_QUERY_RESULT, &timeVal);
#endif

    return timeVal;
}

} // end namespace pic

#endif /* PIC_UTIL_GL_TIMINGS_HPP */

