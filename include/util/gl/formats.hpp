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

#include "../../base.hpp"

#include "../../gl.hpp"

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
 * @param internalFormat is the OpenGL internal format of a texture.
 * @return It returns the number of channels.
 */
inline int getChannelsFromInternalFormatGL(int internalFormat)
{
    int channels = -1;

    switch(internalFormat) {
        //Half precision
        case GL_R16F:
            channels = 1;
            break;

        case GL_RGB16F:
            channels = 3;
            break;

        case GL_RGBA16F:
            channels = 4;
            break;

        //Single precision
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

/**
 * @brief getTextureInformationGL returns width, height and frames values from a
 * texture with target.
 * @param texture is the OpenGL texture pointer.
 * @param target is the OpenGL target of texture.
 * @param width is the horizontal length in pixel of texture.
 * @param height is the vertical length in pixel of texture.
 * @param frames is the number of frames of texture.
 * @param channels is the number of color channels of texture.
 */
PIC_INLINE void getTextureInformationGL(GLuint texture, GLuint target, int &width, int &height,
                             int &frames, int &channels)
{
    if(texture == 0) {
        #ifdef PIC_DEBUG
            printf("getTextureInformationGL: texture is not valid.\n");
        #endif
        return;
    }

    GLint internalFormat;

    switch(target) {
    case GL_TEXTURE_2D: {
        glBindTexture(GL_TEXTURE_2D, texture);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT,
                                 &internalFormat);

        channels = getChannelsFromInternalFormatGL(internalFormat);

        frames = 1;

        glBindTexture(GL_TEXTURE_2D, 0);
    }
    break;

    case GL_TEXTURE_CUBE_MAP: {
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        glGetTexLevelParameteriv(GL_TEXTURE_CUBE_MAP, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(GL_TEXTURE_CUBE_MAP, 0, GL_TEXTURE_HEIGHT, &height);
        glGetTexLevelParameteriv(GL_TEXTURE_CUBE_MAP, 0, GL_TEXTURE_INTERNAL_FORMAT,
                                 &internalFormat);

        channels = getChannelsFromInternalFormatGL(internalFormat);

        frames = 6;

        glBindTexture(GL_TEXTURE_2D, 0);
    }
    break;

    case GL_TEXTURE_2D_ARRAY: {
        glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
        glGetTexLevelParameteriv(GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_HEIGHT, &height);
        glGetTexLevelParameteriv(GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_DEPTH, &frames);
        glGetTexLevelParameteriv(GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_INTERNAL_FORMAT,
                                 &internalFormat);

        channels = getChannelsFromInternalFormatGL(internalFormat);

        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    }
    break;

    case GL_TEXTURE_3D: {
        glBindTexture(GL_TEXTURE_3D, texture);
        glGetTexLevelParameteriv(GL_TEXTURE_3D, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(GL_TEXTURE_3D, 0, GL_TEXTURE_HEIGHT, &height);
        glGetTexLevelParameteriv(GL_TEXTURE_3D, 0, GL_TEXTURE_DEPTH, &frames);
        glGetTexLevelParameteriv(GL_TEXTURE_3D, 0, GL_TEXTURE_INTERNAL_FORMAT,
                                 &internalFormat);

        channels = getChannelsFromInternalFormatGL(internalFormat);

        glBindTexture(GL_TEXTURE_3D, 0);
    }
    break;
    }
}

} // end namespace pic

#endif /* PIC_UTIL_GL_FORMATS_HPP */

