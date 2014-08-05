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

#ifndef PIC_UTIL_GL_FORMATS_HPP
#define PIC_UTIL_GL_FORMATS_HPP

#include "gl.hpp"

namespace pic {

inline void getModesGL(int channels, int *mode, int *modeInternalFormat)
{
    *mode = 0;
    *modeInternalFormat = 0;

    switch(channels) {
    case 1: {
        *mode = GL_RED;
        *modeInternalFormat = GL_R32F;
    }
    break;

    case 3: {
        *mode = GL_RGB;
        *modeInternalFormat = GL_RGB32F;
    }
    break;

    case 4: {
        *mode = GL_RGBA;
        *modeInternalFormat = GL_RGBA32F;
    }
    break;
    }
}

inline void getModesHalfGL(int channels, int *mode, int *modeInternalFormat)
{
    *mode = 0;
    *modeInternalFormat = 0;

    switch(channels) {
    case 1: {
        *mode = GL_RED;
        *modeInternalFormat = GL_R16F;
    }
    break;

    case 3: {
        *mode = GL_RGB;
        *modeInternalFormat = GL_RGB16F;
    }
    break;

    case 4: {
        *mode = GL_RGBA;
        *modeInternalFormat = GL_RGBA16F;
    }
    break;
    }
}

inline void getModesIntegerGL(int channels, int *mode, int *modeInternalFormat)
{
    *mode = 0;
    *modeInternalFormat = 0;

    switch(channels) {
    case 1: {
        *mode = GL_RED_INTEGER;
        *modeInternalFormat = GL_R32I;
    }
    break;

    case 3: {
        *mode = GL_RGB_INTEGER;
        *modeInternalFormat = GL_RGB32I;
    }
    break;

    case 4: {
        *mode = GL_RGBA_INTEGER;
        *modeInternalFormat = GL_RGBA32I;
    }
    break;
    }
}

/**getChannelsFromInternalFormat: returns the number of channels given an internal format*/
inline int getChannelsFromInternalFormatGL(int internalFormat)
{
    int channels = -1;

    switch(internalFormat) {
        case GL_R16F:
            channels = 1;
            break;

        case GL_RGB16F:
            channels = 3;
            break;

        case GL_RGBA16F:
            channels = 4;
            break;

        case GL_R32F:
            channels = 1;
            break;

        case GL_RGB32F:
            channels = 3;
            break;

        case GL_RGBA32F:
            channels = 4;
            break;
        }

    return channels;
}

} // end namespace pic

#endif /* PIC_UTIL_GL_FORMATS_HPP */

