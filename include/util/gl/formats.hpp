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

#ifndef PIC_UTIL_GL_FORMATS_HPP
#define PIC_UTIL_GL_FORMATS_HPP

#include "gl.hpp"

namespace pic {

/**
 * @brief getModesGL
 * @param channels
 * @param mode
 * @param modeInternalFormat
 */
inline void getModesGL(int channels, int &mode, int &modeInternalFormat)
{
    mode = 0;
    modeInternalFormat = 0;

    switch(channels) {
    case 1: {
        mode = GL_RED;
        modeInternalFormat = GL_R32F;
    }
    break;

    case 3: {
        mode = GL_RGB;
        modeInternalFormat = GL_RGB32F;
    }
    break;

    case 4: {
        mode = GL_RGBA;
        modeInternalFormat = GL_RGBA32F;
    }
    break;
    }
}

/**
 * @brief getModesHalfGL
 * @param channels
 * @param mode
 * @param modeInternalFormat
 */
inline void getModesHalfGL(int channels, int &mode, int &modeInternalFormat)
{
    mode = 0;
    modeInternalFormat = 0;

    switch(channels) {
    case 1: {
        mode = GL_RED;
        modeInternalFormat = GL_R16F;
    }
    break;

    case 3: {
        mode = GL_RGB;
        modeInternalFormat = GL_RGB16F;
    }
    break;

    case 4: {
        mode = GL_RGBA;
        modeInternalFormat = GL_RGBA16F;
    }
    break;
    }
}

/**
 * @brief getModesIntegerGL
 * @param channels
 * @param mode
 * @param modeInternalFormat
 */
inline void getModesIntegerGL(int channels, int &mode, int &modeInternalFormat)
{
    mode = 0;
    modeInternalFormat = 0;

    switch(channels) {
    case 1: {
        mode = GL_RED_INTEGER;
        modeInternalFormat = GL_R32I;
    }
    break;

    case 3: {
        mode = GL_RGB_INTEGER;
        modeInternalFormat = GL_RGB32I;
    }
    break;

    case 4: {
        mode = GL_RGBA_INTEGER;
        modeInternalFormat = GL_RGBA32I;
    }
    break;
    }
}

/**
 * @brief getChannelsFromInternalFormatGL returns the number of channels given an internal format.
 * @param internalFormat
 * @return
 */
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

